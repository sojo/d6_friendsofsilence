<?php

/**
 * Helper to implementation of hook_node_info().
 */
function _photo_feature_node_info() {
  $items = array(
    'photo' => array(
      'name' => t('Photo'),
      'module' => 'features',
      'description' => t('A photo which is uploaded and placed in a gallery.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '0',
      'body_label' => '',
      'min_word_count' => '0',
      'help' => '',
    ),
    'photogallery' => array(
      'name' => t('Photo Gallery'),
      'module' => 'features',
      'description' => t('A gallery which contains photos and displays them in a grid view.'),
      'has_title' => '1',
      'title_label' => t('Gallery Title'),
      'has_body' => '1',
      'body_label' => t('Gallery Description'),
      'min_word_count' => '25',
      'help' => '',
    ),
  );
  return $items;
}
