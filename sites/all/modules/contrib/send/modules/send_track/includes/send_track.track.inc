<?php

/**
 * @file
 *
 * Functions related to tracking send's message activity.
 */

/**
 * Menu callback for default tracking activities.
 */
function send_track_page($op, $send_recipient) {
  $args = func_get_args();
  $path = join('/', array_slice($args, 2));

  // Call the API function that manages tracking activities.
  send_track($op, NULL, $send_recipient, $path);

  // The rest of the path is presumably a URL to redirect to.
  drupal_goto($path);
}

/**
 * Menu callback for clickthru tracking.
 * It's a custom callback because the redirect handling is different:
 * we are redirecting to a link object's URL, not to the remainder
 * of the path.
 */
function send_track_page_click($send_recipient, $link) {
  global $base_url;
  send_track('click', NULL, $send_recipient, $link->url);

  if ($link->local) {
    $info = parse_url($base_url . base_path() . $link->url);
    $path = preg_replace('@^' . base_path() . '@', '', $info['path']);
  }
  else {
    $info = parse_url($link->url);
    $path = $info['scheme'] . '://' . $info['host'] . $info['path'];
  }

  $query = isset($info['query']) ? $info['query'] : NULL;
  $fragment = isset($info['fragment']) ? $info['fragment'] : NULL;

  drupal_goto($path, $query, $fragment);
}

/**
 * Schema alter code for the send_recipient table.
 * Handlers can optionally create additional fields for tracking purposes.
 */
function _send_track_schema_alter(&$schema) {
  $table = 'send_recipient';

  // The table doesn't exist at all.
  $new = !db_table_exists($table);

  foreach (module_invoke_all('send_track_handler_info') as $info) {
    if (isset($info['database columns'])) {

      $indexes = array();
      if (isset($info['database indexes'])) {
        $indexes = array('indexes' => $info['database indexes']);
        $schema[$table]['indexes'] = array_merge($schema[$table]['indexes'], $info['database indexes']);
      }

      foreach ($info['database columns'] as $name => $spec) {
        // Add this column to the schema array.
        $schema[$table]['fields'][$name] = $spec;

        // Add the column to the database if it does not already exist.
        if (!$new && !db_column_exists($table, $name)) {
          db_add_field($ret, $table, $name, $spec, $indexes);
          $indexes = array();
        }
      }
    }
  }
}

function send_track($op, $value = 1, $send_recipient = NULL, $path = NULL) {
  if (!$send_recipient) {
    // Look for send_recipient info that was previously set in the session.
    if (isset($_SESSION['send_recipient'])) {
      $send_recipient = send_recipient_load($_SESSION['send_recipient']);
    }
    // Look for a send_hash cookie.
    elseif (isset($_COOKIES['send_hash'])) {
      $send_recipient = send_hash_load($_COOKIES['send_hash']);
    }
  }
  if (!$send_recipient) {
    // This activity has nothing to do with any send activity.  Abort!
    return FALSE;
  }

  if (!$path) $path = $_GET['q'];
  
  // Make sure this is a valid activity.
  $handlers = module_invoke_all('send_track_handler_info');
  if (!isset($handlers[$op])) return FALSE;
  $info = $handlers[$op];

  // Set uniqueness.
  $sql = "SELECT 1 FROM {send_track} WHERE first = 1 AND activity = '%s' AND srid = %d";
  $args = array($op, $send_recipient->srid);

  // If this recipient represents more than 1 person, count unique by IP.
  if ($send_recipient->contact_count > 1) {
    $sql .= " AND hostname = '%s'";
    $args[] = ip_address();
  }
  $first = (int) !(bool) db_result(db_query($sql, $args));
  
  // Store the activity in the send tracking table.
  $send_track = (object) array(
    'srid' => $send_recipient->srid,
    'activity' => check_plain($op),
    'first' => $first,
    'timestamp' => time(),
    'hostname' => ip_address(),
    'path' => filter_xss($path),
  );
  drupal_write_record('send_track', $send_track);
  
  // Let the predefined callback respond to this.
  if (isset($info['callback']) && function_exists($func = $info['callback'])) {
    $func($send_track);
  }

  // Update aggregate totals.
  send_track_aggregate($send_recipient, $op);

  // Let other modules react to this.
  module_invoke_all('send_track', $op, $send_track, $path);

  return $send_track;
}

/**
 * Helper function to update the aggregate tables for tracking counts.
 * This would allow you to purge send_track records without losing stats.
 */
function send_track_aggregate($recipient, $activity) {
  $sql = "SELECT COUNT(*) FROM {send_track} WHERE activity = '%s' AND srid = %d";
  $args = array($activity, $recipient->srid);

  if ($total = db_result(db_query($sql, $args))) {
    $unique = db_result(db_query($sql . ' AND first = 1', $args));
    $values = array(
      'srid' => $recipient->srid,
      $activity => $total,
      $activity . '_unique' => $unique,
    );
    drupal_write_record('send_recipient', $values, array('srid'));
  }
}

/**
 * Implementation of my own hook_send_track_handler_info().
 */
function send_track_send_track_handler_info() {
  return array(
    'open' => array(
      'title' => t('Open rate'),
      'description' => t('An open-rate tracker.'),
      'callback' => 'send_track_open',
      'percentage' => array('open_unique'), // Column(s) that can be expressed as percentages of the total.
      'database columns' => array(
        'open_unique' => array('title' => t('Unique'), 'type' => 'int', 'not null' => TRUE, 'default' => 0),
        'open' => array('title' => t('Total'), 'type' => 'int', 'not null' => TRUE, 'default' => 0),
      ),
    ),
    'click' => array(
      'title' => t('Click-through'),
      'description' => t('Track the number of times an embedded URL is clicked.'),
      'callback' => 'send_track_click',
      'summary callback' => 'send_track_click_summary',
      'percentage' => array('click_unique'), // Column(s) that can be expressed as percentages of the total.
      'database columns' => array(
        'click_unique' => array('title' => t('Unique'), 'type' => 'int', 'not null' => TRUE, 'default' => 0),
        'click' => array('title' => t('Total'), 'type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
      ),
    )
  );
}

/**
 * Implementation of Send module's hook_message_alter().
 */
function _send_track_send_message_alter(&$message, $profile, $r, $nids, $mode) {
  if ($mode == 'mail' || $mode == 'news') { // TODO eliminate mlm and handle elsewhere.
    send_track_message_open($message, $profile, $r, $nids);
    send_track_message_click($message, $profile, $r, $nids);
  }
}

/**
 * Helper function for hook_send_message_alter: open rate.
 */
function send_track_message_open(&$message, $profile, $r, $nids) {
  global $base_url;

  // Open:  We'll only replace one src image to avoid overrunning the server.
  $open_path  = 'send/o/'. $r->hash .'/';
  $pattern = '@(src=")('. $base_url .')?('. base_path() .')(.*q=)?([^"]+)@mi';
  $message['body'] = preg_replace($pattern, '\1' . $base_url . base_path() . $open_path .'\5', $message['body'], 1);
}

/**
 * Helper function for hook_send_message_alter: click-through.
 */
function send_track_message_click(&$message, $profile, $r, $nids) {
  // Put $r in the global context so it's availble in my callback.
  // TODO I know this is a sucky thing to do.
  global $recipient;
  $recipient = $r;

  $pattern = '@(href=")([^"]+)@mi';
  $message['body'] = preg_replace_callback($pattern, 'send_track_replace_click', $message['body']);
}

/**
 * preg callback to translate message links to stored links.
 */
function send_track_replace_click($matches) {
  // See comment in send_track_message_click().
  global $recipient;

  $link = send_track_click_link($matches[2]);

  // Click-through path includes user hash + link hash.
  return $matches[1] . base_path() . 'send/c/'. $recipient->hash .'/' . $link->hash;
}

/**
 * Helper function to create/load a link.
 */
function send_track_click_link($url) {
  global $base_url;

  // Is this a fully-qualified URL?
  if (preg_match('/^http/i', $url)) {

    // Is the URL a local one?
    if (strpos($url, $base_url)) {
      // Remove the site's URL from the link.
      preg_replace('@' . $base_url . base_path() . '@', '', $url);

      // Denote it as local.
      $local = TRUE;
    }
    // No? then it's a remote URL.
    else {
      $local = FALSE;
    }
  }
  else {
    $url = preg_replace('@^' . base_path() . '@', '', $url);
    $local = TRUE;
  }

  // See if we can translate the path.
  if ($local) {
    $url = drupal_get_normal_path($url);
  }

  if ($link = db_fetch_object(db_query("SELECT * FROM {send_track_link} WHERE url = '%s'", $url))) {
    return $link;
  }
  else {
    $link = (object) array(
      'url' => $url,
      'local' => (int) $local, 
      'hash' => md5($url . time()),
    );

    drupal_write_record('send_track_link', $link);
    return $link;
  }
}

/**
 * Custom behavior for the "click" activity.
 */
function send_track_click($send_track) {
  // Store the event in the session:  It may be retrieved for, say, a purchase.
  $_SESSION['send_recipient'] = $send_track->srid;
}
