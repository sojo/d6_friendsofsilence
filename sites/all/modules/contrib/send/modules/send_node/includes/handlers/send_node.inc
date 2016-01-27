<?php

/**
 * @file
 *
 * Implement a sendProfile for Send-to-friend activities.
 */

class sendNodeProfile extends sendProfile {
  public $handler = 'send_node';
  public $linktext;
  public $enabled;
  public $default;

  public $sender_editable = TRUE;
  public $body_format_editable = FALSE;

  public function access() {
    // Check against threshold.
    $limit = variable_get('send_node_limit', 5);
    if ($limit && !flood_is_allowed("send node", $limit)) return FALSE;

    // Does the user have the send nodes permission?
    if (!user_access('send nodes')) return FALSE;

    // Check whether we're sending valid nodes for this profile
    foreach ($this->nids as $nid) {
      // Invalid or inaccessible node.
      if (!$node = node_load($nid)) return FALSE;

      // "Send to friend" not enabled for this node type.
      if (!variable_get('send_enabled_'. $node->type, FALSE)) return FALSE;

      // TODO limit for incorrect nid/type matchups with this profile.
    }

    return TRUE;
  }

  public function setEnabled($enabled) {
    $this->enabled = (bool) $enabled;
  }

  public function setDefault($default) {
    $this->default = (bool) $default;
  }

  public function linktext() {
    $this->setLinktext();
    return $this->linktext;
  }

  public function setLinktext($value = NULL) {
    if (!is_null($value)) {
      $this->linktext = ($value == '<none>') ? $value : filter_xss($value);
    }
    elseif (!isset($this->linktext)) {
      $this->linktext = t('send to friend');
    }
  }

  function settings_form(&$form, &$form_state) {
    parent::settings_form($form, $form_state);
    $group = $this->name();

    $form[$group]['linktext'] = array(
      '#type'          => 'textfield',
      '#title'         => t("Link text"),
      '#default_value' => $this->linktext(),
      '#size'          => 30,
      '#maxsize'       => 100,
      '#description'   => t('The text for the link to the email form. Use <em>&lt;none&gt;</em> to hide this link.'),
    );
  }

  function formValidate(&$form, &$form_state) {
    $limit = variable_get('send_node_recipient_limit', 5);
    if (count($this->recipients) > $limit) {
      form_set_error('send[recipients]', t('You may not send messages to more than @limit recipients.', array('@limit' => $limit)));
    }
  }

  function send() {
    // Rely on the main "send" functionality, but register it against limits. 
    if ($count = parent::send()) {
      flood_register_event("send node");

      if ($count == 1) {
        drupal_set_message(t('Message sent to @recipient', array('@recipient' => $this->recipients[0]['mail'])));
      }
      else {
        drupal_set_message(t('Message sent to @count recipients', array('@count' => $count)));
      }
    }
  }
}
