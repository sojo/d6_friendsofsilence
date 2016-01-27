<?php

/**
 * Implementation of hook_content_default_fields().
 */
function home_rotator_feature_content_default_fields() {
  module_load_include('inc', 'home_rotator_feature', 'home_rotator_feature.defaults');
  $args = func_get_args();
  return call_user_func_array('_home_rotator_feature_content_default_fields', $args);
}

/**
 * Implementation of hook_imagecache_default_presets().
 */
function home_rotator_feature_imagecache_default_presets() {
  module_load_include('inc', 'home_rotator_feature', 'home_rotator_feature.defaults');
  $args = func_get_args();
  return call_user_func_array('_home_rotator_feature_imagecache_default_presets', $args);
}

/**
 * Implementation of hook_node_info().
 */
function home_rotator_feature_node_info() {
  module_load_include('inc', 'home_rotator_feature', 'home_rotator_feature.features.node');
  $args = func_get_args();
  return call_user_func_array('_home_rotator_feature_node_info', $args);
}

/**
 * Implementation of hook_views_default_views().
 */
function home_rotator_feature_views_default_views() {
  module_load_include('inc', 'home_rotator_feature', 'home_rotator_feature.features.views');
  $args = func_get_args();
  return call_user_func_array('_home_rotator_feature_views_default_views', $args);
}
