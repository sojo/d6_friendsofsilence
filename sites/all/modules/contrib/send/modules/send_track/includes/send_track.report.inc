<?php

/**
 * @file
 *
 * Functions related to reporting send's message activity.
 */

/**
 * A complete listing of each click/open/etc. for a message.
 */
function send_track_activity_details($message, $activity) {
  $info = send_track_handler_info();
  $title = $info[$activity]['title'];
  drupal_set_title(t('@activity report', array('@activity' => $title)));

  $res = pager_query("SELECT st.*, sr.*, sc.uid, sc.mode FROM {send_track} st
    INNER JOIN {send_recipient} sr USING ( srid )
    INNER JOIN {send_contact} sc USING ( scid )
    WHERE sr.sid = %d AND st.activity = '%s'
    ORDER BY st.timestamp DESC", 100, 0, NULL, $message->sid, $activity);

  $headers = array(
    t('Date'),
    t('User'),
    t('Hostname'),
    t('URL'),
  );

  $rows = array();
  while ($row = db_fetch_object($res)) {
    $account = user_load($row->uid);
    $menu = menu_get_item($row->path);
    $title = isset($menu['title']) ? $menu['title'] : $row->path;

    $rows[] = array(
      format_date($row->timestamp, 'short'),
      theme_username($account),
      $row->hostname,
      l($title, $row->path),
    );
  }

  return theme('table', $headers, $rows);
}

function send_track_click_summary($message) {
  $headers = array(
    t('URL'),
    t('Total clicks'),
  );
  $rows = array();

  $total = db_result(db_query("SELECT COUNT(st.path) 
    FROM {send_track} st
    INNER JOIN {send_recipient} sr USING ( srid )
    WHERE sr.sid = %d AND st.activity = 'click'
    ", $message->sid));

  // No clicks registered.  Do nothing.
  if (!$total) return;

  $res = db_query("SELECT st.path, COUNT(st.path) AS count
    FROM {send_track} st
    INNER JOIN {send_recipient} sr USING ( srid )
    WHERE sr.sid = %d AND st.activity = 'click'
    GROUP BY st.path
    ORDER BY COUNT(st.path) DESC
    ", $message->sid);

  while ($row = db_fetch_object($res)) {
    $menu = menu_get_item($row->path);
    $title = isset($menu['title']) ? $menu['title'] : $row->path;
    $percent = round($row->count / $total * 100);

    $rows[] = array(
      l($title, $row->path),
      $row->count .' ('. $percent .'%)',
    );
  }
  return array(
    'title' => t('Click summary'),
    'value' => theme('table', $headers, $rows)
  );
}
