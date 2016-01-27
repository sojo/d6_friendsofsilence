<?php



/**
 * Implementation of hook_rules_event_info().
 * @ingroup rules
 */
function node_send_rules_event_info() {
  return array(
    'node_send_email_node' => array(
      'label' => t('User Sends Node to Themselves'),
      'module' => 'Node Send',
      'arguments' => array(
        'sender' => array('type' => 'user', 'label' => t('Acting User')),
        'node' => array('type' => 'node', 'label' => t('Node To Send')),
      ),
    ),
    'node_send_email_group' => array(
      'label' => t('Email Node To Group'),
      'module' => 'Node Send',
      'arguments' => array(
        'node' => array('type' => 'node', 'label' => t('Node To Send')),
        'sender' => array('type' => 'user', 'label' => t('Sending User')),
      ),
    ),
  );
}


/**
 * Implementation of hook_rules_action_info().
 */
function node_send_rules_action_info() {
  return array(
      'node_send_action_mail_to_group' => array(
      'label' => t('Send a mail to users of a group'),
      'module' => 'Node Send',
      'eval input' => array('gid', 'subject', 'message', 'from'),
    ),
  );
}

/**
 * Action: Send mail to all users of the specified group
 */
function node_send_action_mail_to_group($settings) {
    //dpm($settings);
    $members = array();
    $sql = 'SELECT uid FROM {og_uid} WHERE nid = "%s"';
    $theresult = db_query(db_rewrite_sql($sql), $settings['gid']);
    while ($data = db_fetch_object($theresult)) {
    	$user = NULL;
    	$user = user_load($data->uid);
    	$members[] = $user;
    }
    foreach($members as $member) {
      $headers = array(
        'From'    => $settings['from']
      );
      //send via mimemail
      mimemail($settings['from'], $member->mail, $settings['subject'], $settings['message'], $plaintext = NULL, $headers, $text = NULL, $attachments = array(), $mailkey = 'node_send_group_send', $send = TRUE);
    }
}