<?php

/**
 * Helper to implementation of hook_node_info().
 */
function _home_rotator_feature_node_info() {
  $items = array(
    'feature' => array(
      'name' => t('Homepage Feature'),
      'module' => 'features',
      'description' => t('A featured image with link that is added to the homepage'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '0',
      'body_label' => '',
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}
