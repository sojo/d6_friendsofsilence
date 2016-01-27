<?php

/**
 * @file
 *
 * Menu functions for the Send API.
 */

function send_menu_menu() {
  $path = drupal_get_path('module', 'send') .'/includes';
  $items = array(
    'send/%send_profile/%' => array(
      'title'            => 'Send',
      'page callback'    => 'drupal_get_form',
      'page arguments'   => array('send_form', 1, 2),
      'access callback'  => 'send_access',
      'access arguments' => array(1, 2),
      'type'             => MENU_CALLBACK,
      'file'             => 'send.form.inc',
      'file path'        => $path,
    ),
    'admin/settings/send_template' => array(
      'title'            => 'E-mail Message templates',
      'description'      => 'Control the appearance of HTML email for newsletters and other mailings that use the Send API.',
      'page callback'    => 'send_template_admin_overview',
      'access arguments' => array('administer send'),
      'type'             => MENU_NORMAL_ITEM,
      'file'             => 'send.template.inc',
      'file path'        => $path,
    ),
    'admin/settings/send_template/list' => array(
      'title'            => 'List',
      'page callback'    => 'send_template_admin_overview',
      'type'             => MENU_DEFAULT_LOCAL_TASK,
      'weight'           => -10,
    ),
    'admin/settings/send_template/add' => array(
      'title'            => 'Add',
      'page callback'    => 'drupal_get_form',
      'page arguments'   => array('send_template_form'),
      'access arguments' => array('administer send'),
      'type'             => MENU_LOCAL_TASK,
      'file'             => 'send.template.inc',
      'file path'        => $path,
    ),
    'admin/settings/send_template/%/edit' => array(
      'title'            => 'Edit',
      'page callback'    => 'drupal_get_form',
      'type'             => MENU_CALLBACK,
      'page arguments'   => array('send_template_form', 3),
      'access arguments' => array('administer send'),
      'file'             => 'send.template.inc',
      'file path'        => $path,
    ),
    'send/js/send_form' => array(
      'page callback'    => 'send_form_js',
      'access arguments' => array('administer send'), // TODO not correct.
      'file'             => 'send.form.inc',
      'file path'        => $path,
    ),
    'node/%node/send' => array(  //TODO this isn't done yet.
    ),
  );
  return $items;
}
