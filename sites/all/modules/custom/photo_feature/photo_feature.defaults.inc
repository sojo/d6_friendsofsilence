<?php

/**
 * Helper to implementation of hook_content_default_fields().
 */
function _photo_feature_content_default_fields() {
  $fields = array();

  // Exported field: field_gallery
  $fields[] = array(
    'field_name' => 'field_gallery',
    'type_name' => 'photo',
    'display_settings' => array(
      'weight' => '-4',
      'parent' => '',
      'label' => array(
        'format' => 'above',
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '2' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '3' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'nodereference',
    'required' => '1',
    'multiple' => '0',
    'module' => 'nodereference',
    'active' => '1',
    'referenceable_types' => array(
      'photogallery' => 'photogallery',
      'audio' => 0,
      'blog' => 0,
      'event' => 0,
      'growth' => 0,
      'homemiddlebanner' => 0,
      'ministry' => 0,
      'news' => 0,
      'page' => 0,
      'photo' => 0,
      'sermonseries' => 0,
      'offsite_video' => 0,
      'webform' => 0,
    ),
    'advanced_view' => '--',
    'advanced_view_args' => '',
    'widget' => array(
      'node_link' => array(
        'teaser' => 0,
        'full' => 1,
        'title' => 'Add A Photo',
        'hover_title' => 'Add a photo to this gallery',
        'destination' => 'node',
      ),
      'fallback' => 'select',
      'edit_fallback' => 1,
      'label' => 'Gallery',
      'weight' => '-4',
      'description' => '',
      'type' => 'nodereference_url',
      'module' => 'nodereference_url',
    ),
  );

  // Exported field: field_image
  $fields[] = array(
    'field_name' => 'field_image',
    'type_name' => 'photo',
    'display_settings' => array(
      'weight' => '-3',
      'parent' => '',
      'label' => array(
        'format' => 'hidden',
      ),
      'teaser' => array(
        'format' => 'gallery-teaser_default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'galleryphotos_default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'hidden',
        'exclude' => 1,
      ),
      '2' => array(
        'format' => 'image_plain',
        'exclude' => 0,
      ),
      '3' => array(
        'format' => 'image_plain',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'image_plain',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'filefield',
    'required' => '1',
    'multiple' => '0',
    'module' => 'filefield',
    'active' => '1',
    'list_field' => '0',
    'list_default' => 1,
    'description_field' => '0',
    'widget' => array(
      'file_extensions' => 'png gif jpg jpeg',
      'file_path' => 'photos',
      'progress_indicator' => 'bar',
      'max_filesize_per_file' => '',
      'max_filesize_per_node' => '',
      'max_resolution' => '0',
      'min_resolution' => '0',
      'alt' => '',
      'custom_alt' => 1,
      'title' => '',
      'custom_title' => 0,
      'title_type' => 'textfield',
      'default_image' => NULL,
      'use_default_image' => 0,
      'label' => 'Photo',
      'weight' => '-3',
      'description' => '',
      'type' => 'imagefield_widget',
      'module' => 'imagefield',
    ),
  );

  // Translatables
  array(
    t('Gallery'),
    t('Photo'),
  );

  return $fields;
}

/**
 * Helper to implementation of hook_imagecache_default_presets().
 */
function _photo_feature_imagecache_default_presets() {
  $items = array(
    'gallery-teaser' => array(
      'presetname' => 'gallery-teaser',
      'actions' => array(
        '0' => array(
          'weight' => '0',
          'action' => 'imagecache_scale_and_crop',
          'data' => array(
            'width' => '150',
            'height' => '100',
          ),
          'module' => 'imagecache',
        ),
      ),
    ),
    'galleryphotos' => array(
      'presetname' => 'galleryphotos',
      'actions' => array(
        '0' => array(
          'weight' => '0',
          'action' => 'imagecache_scale',
          'data' => array(
            'width' => '225',
            'height' => '',
            'upscale' => 1,
          ),
          'module' => 'imagecache',
        ),
      ),
    ),
  );
  return $items;
}
