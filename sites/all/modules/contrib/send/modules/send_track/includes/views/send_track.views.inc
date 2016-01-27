<?php

/**
 * @file
 *
 * Default views for the Send Tracking module.
 */
/**
 * Implementation of hook_views_handlers().
 */
function send_track_views_handlers() {
  return array(
    'handlers' => array(
      'send_track_views_handler_field_aggregate' => array(
        'parent' => 'send_views_handler_field_aggregate',
        'path' => drupal_get_path('module', 'send_track') . '/includes/views',
      ),
      'send_track_views_handler_field_link' => array(
        'parent' => 'views_handler_field',
        'path' => drupal_get_path('module', 'send_track') . '/includes/views',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_data().
 */
function send_track_views_data() {
  $data['send_track']['table']['group'] = t('Send');

  // Form a relationship to the send_recipient table.
  $data['send_track']['srid'] = array(
  );

  // Filter / field handler for the activity type.
  // Todo write a "where in" handler.
  $data['send_track']['activity'] = array(
  );

  // Hostname
  $data['send_track']['hostname'] = array(
  );

  // Timestamp
  $data['send_track']['hostname'] = array(
  );

  // TODO the "path" item is really only interesting to e.g. "click" functionality

  $data['send']['send_track_link'] = array(
    'group' => t('Send tracking'),
    'title' => t('Link to report'),
    'help' => t('Tracked statistics and other details for a message.'),
    'field' => array(
      'handler' => 'send_track_views_handler_field_link', 
    ),
  );

  // Add our columns to the send table.
  foreach (send_track_handler_info() as $name => $info) {
    $data['send'][$name] = array(
      'group' => t('Send tracking'),
      'title' => $info['title'],
      'help' => $info['description'],
      'field' => array(
        'handler' => 'send_track_views_handler_field_aggregate', 
        'click sortable' => TRUE,
        'table' => 'send_recipient',
        'field' => $name,
      ),
      'sort' => array(
        'help'    => $info['description'],
        'handler' => 'views_handler_sort',
      ),
    );
  }
  return $data;
}
