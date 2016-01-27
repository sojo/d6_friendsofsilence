<?php

/**
 * @file
 *
 * Theme functions for the Send Tracking module.
 */
function send_track_theme_theme() {
  $path = drupal_get_path('module', 'send_track') .'/theme';
  return array(
    'send_track_message' => array(
      'arguments' => array( 'message' => NULL ),
      'file' => 'send_track.theme.inc',
      'template' => 'send_track_message',
      'path' => $path,
    ),
  );
}

/**
 * Preprocess function for send tracking. adds summary details.
 */
function send_track_preprocess_send_track_message(&$variables, $hook) {
  $message =& $variables['message'];
  $handlers = send_track_handler_info();

  $variables['summary'] = array();
  $variables['details'] = array();

  $variables['summary']['contact_count'] = array();

  $columns = array("SUM(contact_count) AS contact_count");
  $groups = array();

  foreach ($handlers as $activity => $info) {
    if (!isset($info['database columns']) || !is_array($info['database columns'])) {
      return;
    }

    // Build out summary information.
    $variables['summary'][$activity] = array(
      'title' => $info['title'],
      'value' => array(),
    );

    $func = 'SUM'; // TODO
    foreach ($info['database columns'] as $column => $column_info) {
      $columns[$column] = $func . '(' . $column . ') AS `' . $column .'`';
      $variables['summary'][$activity]['data'][$column]['title'] = $column_info['title'];

      // Make a group so we can recall which columns an activity belongs to.
      $groups[$column] = $activity;
    }

    // Build out details, if available.
    if (isset($info['summary callback'])) {
      $func = $info['summary callback'];
      $variables['details'][$activity] = $func($message);
    }
  }

  $columns = join(', ', $columns);

  // Gather the stat summaries from the send_recipient table.
  $row = db_fetch_object(db_query("SELECT $columns FROM {send_recipient} 
    WHERE sid = %d", $message->sid));

  $total = 0;
  foreach ($row as $column => $value) {
    if ($column == 'contact_count') {
      $total = $value;
      $variables['summary']['contact_count'] = array(
      'title' => t('Recipients'),
      'data' => array(
        'count' => array( 'title' => t('Total recipients'), 'value' => $total),
        ),
      );
    }
    else {
      $activity = $groups[$column];
      if ($total && isset($handlers[$activity]['percentage']) && in_array($column, $handlers[$activity]['percentage'])) {
        $percent = round($value / $total * 100, 2);
        $value .= ' (' . $percent . '%)';
      }

      $variables['summary'][$activity]['data'][$column]['value'] = $value;
    }
  }
}
