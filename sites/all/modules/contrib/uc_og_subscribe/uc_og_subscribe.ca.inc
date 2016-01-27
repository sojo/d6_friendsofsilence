<?php
// $Id: uc_og_subscribe.ca.inc,v 1.1 2009/09/25 17:56:09 ezrag Exp $

/**
 * @file
 * Allows products to be associated with group memberships.
 */


/******************************************************************************
 * Conditional Actions Hooks                                                  *
 ******************************************************************************/

/**
 * Implementation of hook_ca_predicate().
 */
function uc_og_subscribe_ca_predicate() {

  // Renew all the groups on an order.
  $configurations['uc_og_subscribe_renewal'] = array(
    '#title' => t('Update group subscriptions'),
    '#description' => t('Update group subscriptions based on order status.'),
    '#class' => 'subscription',
    '#trigger' => 'uc_order_status_update',
    '#status' => 1,
    '#conditions' => array(
      '#operator' => 'AND',
      '#conditions' => array(
        array(
          '#name' => 'uc_order_status_condition',
          '#title' => t('If the order status is completed.'),
          '#argument_map' => array(
            'order' => 'updated_order',
          ),
          '#settings' => array(
            'order_status' => 'completed',
          ),
        ),
      ),
    ),
    '#actions' => array(
      array(
        '#name' => 'uc_og_subscribe_order_renew',
        '#title' => t('Update all group subscriptions.'),
        '#argument_map' => array(
          'order' => 'order',
        ),
      ),
    ),
  );

  // Notify the user when a group is granted.
  $configurations['uc_og_subscribe_notify_grant_trigger'] = array(
    '#title' => t('Notify customer when subscribed to a group'),
    '#description' => t('Notify the customer when they have been subscribed to a group.'),
    '#class' => 'notification',
    '#trigger' => 'uc_og_subscribe_notify_grant',
    '#status' => 1,
    '#actions' => array(
      array(
        '#name' => 'uc_og_subscribe_order_email',
        '#title' => t('Send an e-mail to the customer'),
        '#argument_map' => array(
          'order' => 'order',
          'groups' => 'groups',
        ),
        '#settings' => array(
          'from' => uc_store_email_from(),
          'addresses' => '[order-email]',
          'subject' => t("Group subscriptions for Order# [order-id]"),
          'message' => t("Your order (order# [order-link]) at [store-name] included group subscription(s). You may access them with the following link(s):\n[group-links]\n"),
          'format' => 1,
        ),
      ),
    ),
  );

  return $configurations;
}

/**
 * Implementation of hook_ca_entity().
 *
 * An entity is defined in order to get group links down
 * to token in the email.
 */
function uc_og_subscribe_ca_entity() {
  $entities['uc_og_subscribe_groups'] = array(
    '#title' => t('Organic group(s)'),
    '#type' => 'array',
  );

  return $entities;
}

/**
 * Implementation of hook_ca_action().
 */
function uc_og_subscribe_ca_action() {

  $actions['uc_og_subscribe_order_renew'] = array(
    '#title' => t('Renew the groups on an order'),
    '#category' => t('renewal'),
    '#callback' => 'uc_og_subscribe_action_order_renew',
    '#arguments' => array(
      'order' => array(
        '#entity' => 'uc_order',
        '#title' => t('Order'),
      ),
    ),
  );

  // Send an email to an order with group links(s)
  $actions['uc_og_subscribe_order_email'] = array(
    '#title' => t('Send an order email regarding group subscriptions'),
    '#category' => t('Notification'),
    '#callback' => 'uc_og_subscribe_action_order_email',
    '#arguments' => array(
      'order' => array(
        '#entity' => 'uc_order',
        '#title' => t('Order'),
      ),
      'groups' => array(
        '#entity' => 'uc_og_subscribe_groups',
        '#title' => t('Groups'),
      ),
    ),
  );

  return $actions;
}

/**
 * Implementation of hook_ca_trigger().
 */
function uc_og_subscribe_ca_trigger() {
  $triggers['uc_og_subscribe_notify_grant'] = array(
    '#title' => t('E-mail for group subscription'),
    '#category' => t('Notification'),
    '#arguments' => array(
      'order' => array(
        '#entity' => 'uc_order',
        '#title' => t('Order'),
      ),
      'groups' => array(
        '#entity' => 'uc_og_subscribe_groups',
        '#title' => t('Groups'),
      ),
    ),
  );

  return $triggers;
}

/**
 * Send an email with order and group link tokens.
 *
 * The recipients, subject, and message fields take order token replacements.
 *
 * @see uc_og_subscribe_action_order_email_form()
 */
function uc_og_subscribe_action_order_email($order, $groups, $settings) {
  $account = uc_order_user_load($order);

  // Token replacements for the subject and body
  $settings['replacements'] = array(
    'global' => NULL,
    'order' => $order,
    'user' => $account,
    'uc_og_subscribe_groups' => $groups,
  );

  // Replace tokens and parse recipients.
  $recipients = array();
  $addresses = token_replace_multiple($settings['addresses'], $settings['replacements']);
  foreach (explode("\n", $addresses) as $address) {
    $recipients[] = trim($address);
  }

  // Send to each recipient.
  foreach ($recipients as $email) {
    $sent = drupal_mail('uc_order', 'action-mail', $email, uc_store_mail_recipient_language($email), $settings, $settings['from']);

    if (!$sent['result']) {
      watchdog('ca', 'Attempt to e-mail @email concerning order @order_id failed.', array('@email' => $email, '@order_id' => $order->order_id), WATCHDOG_ERROR);
    }
  }
}

/**
 * @see uc_og_subscribe_action_order_email()
 */
function uc_og_subscribe_action_order_email_form($form_state, $settings = array()) {
  return uc_og_subscribe_build_email_form($form_state, $settings, array('global', 'uc_og_subscribe_groups'));
}

/**
 * Build an email action settings form.
 */
function uc_og_subscribe_build_email_form($form_state, $settings, $token_filters) {
  $form = array();

  $form['from'] = array(
    '#type' => 'textfield',
    '#title' => t('Sender'),
    '#default_value' => $settings['from'],
    '#description' => t('The "From" address.'),
    '#required' => TRUE,
  );
  $form['addresses'] = array(
    '#type' => 'textarea',
    '#title' => t('Recipients'),
    '#default_value' => $settings['addresses'],
    '#description' => t('Enter the email addresses to receive the notifications, one on each line. You may use order tokens for dynamic email addresses.'),
    '#required' => TRUE,
  );
  $form['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => $settings['subject'],
    '#required' => TRUE,
  );
  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#default_value' => $settings['message'],
  );

  // We add the #is_format element to allow us to locate and configure the filters.
  $form['format'] = filter_form($settings['format']) + array('#is_format' => TRUE);

  $form['token_help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Replacement patterns'),
    '#description' => t('You can make use of the replacement patterns in the recipients field, the subject, and the message body.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  foreach ($token_filters as $name) {
    $form['token_help'][$name] = array(
      '#type' => 'fieldset',
      '#title' => t('@name replacement patterns', array('@name' => drupal_ucfirst($name))),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['token_help'][$name]['content'] = array(
      '#value' => theme('token_help', $name),
    );
  }

  return $form;
}

/**
 * Renew an orders group subscriptions.
 *
 * @param $order
 *   An Ubercart order object.
 */
function uc_og_subscribe_action_order_renew($order) {
  // Load user.
  if (!$order->uid || !($order_user = user_load($order->uid))) {
    return;
  }

  $user_groups = array();
  // Scan products for models matching downloads.
  foreach ($order->products as $product) {
    $groups = db_query("SELECT * FROM {uc_og_subscribe} AS uos ".
      "INNER JOIN {uc_product_features} AS pf ON pf.pfid = uos.pfid ".
      "WHERE nid = %d", $product->nid);

    while ($group = db_fetch_object($groups)) {

      // Either they match, or the model was set to any SKU.
      if (!empty($group->model) && $group->model != $product->model) {
        continue;
      }

      $group_info = db_fetch_object(db_query_range('SELECT * FROM {og} AS og INNER JOIN {node} AS n ON og.nid = n.nid WHERE og.nid = %d', array($group->gid), 0, 1));

      $new_group = array(array($group_info->nid => $group_info->title));

      og_save_subscription($group->gid, $order_user->uid, array('is_active' => $group->is_active, 'is_admin' => $group->is_admin));

      // Save for notification.
      $user_groups = array_merge($user_groups, $new_group);

      // Note on the order where the user has gained group membership.
      $comment = t('User can now access the group: %group.', array('%group' => filter_xss_admin($group_info->title)));
      uc_order_comment_save($order->order_id, $user->uid, $comment);
    }
  }

  // Notify the user of their download(s).
  if ($user_groups) {
    ca_pull_trigger('uc_og_subscribe_notify_grant', $order, $user_groups);
  }
}
