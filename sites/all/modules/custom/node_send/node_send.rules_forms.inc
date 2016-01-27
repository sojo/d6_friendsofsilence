<?php


/**
 * Action "Send a mail to a user" configuration form
 */
function node_send_action_mail_to_group_form($settings = array(), &$form) {
  $settings += array('gid' => '', 'from' => '', 'subject' => '', 'message' => '');
  $form['settings']['gid'] = array(
    '#type' => 'textfield',
    '#title' => t('Group ID'),
    '#default_value' => $settings['gid'],
    '#description' => t("The Group ID for the node thats sent"),
  );
  $form['settings']['from'] = array(
    '#type' => 'textfield',
    '#title' => t('Sender'),
    '#default_value' => $settings['from'],
    '#description' => t("The mail's from address. Leave it empty to use the site-wide configured address."),
  );
  $form['settings']['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => $settings['subject'],
    '#description' => t("The mail's subject."),
  );
  $form['settings']['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#default_value' => $settings['message'],
    '#description' => t("The mail's message body."),
  );
}