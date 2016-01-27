<?php

/**
 * @file
 *
 * Theme functions for the Send module.
 */

function send_theme_theme() {
  $path = drupal_get_path('module', 'send') .'/theme';
  return array(
    'send' => array(
      'arguments' => array( 'element' => NULL ),
      'file' => 'send.theme.inc',
      'template' => 'send',
      'path' => $path,
    ),
    'send_element' => array(
      'arguments' => array( 'element' => NULL ),
      'file' => 'send.theme.inc',
      'path' => $path,
    ),
    'send_message_compose_element' => array(
      'arguments' => array( 'element' => array() ),
      'file' => 'send.theme.inc',
      'path' => $path,
    ),
    'send_body' => array(
      'arguments' => array( 'values' => array() ),
      'file' => 'send.theme.inc',
      'path' => $path,
    ),
    'send_draggable' => array(
      'arguments' => array( 'element' => array() ),
      'file' => 'send.theme.inc',
      'path' => $path,
    ),
  );
}

function send_preprocess(&$variables, $hook) {
  if (isset($variables['element']['sender'])) {
    $variables['sender'] = $variables['element']['sender']['#children'];
  }
  else $variables['sender'] = '';

  if (isset($variables['element']['recipients'])) {
    $variables['recipients'] = $variables['element']['recipients']['#children'];
  }
  else $variables['recipients'] = '';

  if (isset($variables['element']['buttons'])) $variables['buttons'] = $variables['element']['buttons']['#children'];
  if (isset($variables['element']['content']['#children'])) {
    $variables['content'] = $variables['element']['content']['#children'];
  }
  $variables['extra'] = '';
  if (isset($variables['element']['#children'])) {
    $variables['extra'] = $variables['element']['#children'];
  }

  $variables['build_type'] = 'default';
  if (isset($variables['element']['content']['#send_build_type']) && $variables['element']['content']['#send_build_type'] == 'items') {
    $variables['build_type'] = 'content';
  }
}

/**
 * The default theme function any other "send" element.
 */
function theme_send_element($element) {
  if ($output = drupal_render($element)) {
    return theme('form_element', $element, $output);
  }
}

function theme_send_message_compose_element($element) {
  $admin   = drupal_render($element['admin']);
  $content = drupal_render($element['content']);
  $extra = drupal_render($element);

  $output = '';
  if ($content || $extra) {
    $output .=  $extra . $content;

    if (isset($element['#send_build_type']) && $element['#send_build_type'] != 'items') {
      $output = '<div id="message-content" style="width:70%; float: left; margin-right: 20px;">'
        . $output
        . '</div>';
    }
  }
  if ($admin) {
    $output .= $admin;
  }
  if ($output) {
    $output .= '<br clear="all" />';
  }
  return $output;
}

function theme_send_body($body, $profile, $values = array()) {
  $build_mode = isset($profile->build_mode) ? $profile->build_mode : 'teaser';

  // The content may have been supplied. If not, generate!
  if ($profile->content()) {
    $content = '';
    $i = 0;
    foreach ($profile->content() as $name => $data) {
      // Special-case the 'message' element.
      if ($name == 'message') {
        $values[$name] = $profile->processText($data['content']);
        continue;
      }
      elseif (isset($data['nid'])) {
        $delta = $i+1;
        $node = node_load($data['nid']);
        if (!isset($data['content'])) {
          $node->build_mode = isset($data['build_mode']) ? $data['build_mode'] : $build_mode;
          $teaser = $node->build_mode == 'teaser';
          $data['content'] = node_view($node, $teaser);
        }
      }
      if (isset($data['content'])) {
        $content .= $data['content'];
      }
    }
    $values['body'] = $content;
  }

  foreach ($values as $key => $value) {
    if ($key && is_scalar($value)) {
      $tokens[] = '%'. $key;
      $replace[] = $value;
    }
  }
  $body = str_replace($tokens, $replace, $body);

  $body =  $profile->processText($body, $profile->body_format);
  return $body;
}

function theme_send_draggable($element) {
  if (!isset($element['#id'])) $element['#id'] = '';
  $id = $element['#id'] . '-table';
  $weight_class = $element['#id'] .'-weight';

  $rows = array();
  foreach (element_children($element) as $key) {
    $weight = "";
    if (isset($element[$key]['weight'])) {
      $element[$key]['weight']['#attributes']['class'] = $weight_class;
      $weight = drupal_render($element[$key]['weight']);
    }
    $content = drupal_render($element[$key]);

    if ($weight) {
      $rows[] = array(
        'data' => array($content, $weight),
        'class' => 'draggable',
      );
    }
    else {
      $rows[] = array($content);
    }
  }

  drupal_add_tabledrag($id, 'order', 'sibling', $weight);

  $output = theme('table', array(), $rows, array('id' => $id));
  return $output;
}
