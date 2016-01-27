<?php

/**
 * @file
 *
 * Functions that provide form elements for various aspects of 
 * Send API forms.
 */

/**
 * The default submission page for user-initiated send activities.
 */
function send_form($form_state, $profile, $nids = array()) {
  // Prevent this page from getting indexed.
  header('X-Robots-Tag: noindex,noarchive', TRUE);

  $profile->nids = ($nids && !is_array($nids)) ? explode(' ', $nids) : $nids;

  return array(
    'send' => array(
      '#type' => 'send',
      '#send_profile' => $profile,
    ),
    'buttons' => array(
      'submit' => array( '#type' => 'submit', '#value' => t('Send') ),
    ),
  );
}

/**
 * A validate handler that is used for any form that contains a send element.
 * Added by send_element_process().
 */
function send_validate($form, &$form_state) {
  $form_state['storage']['send_profile']->formValidate($form, $form_state);
}

/**
 * A submit handler that is used for any form that contains a send element.
 * Added by send_element_process().
 */
function send_submit($form, &$form_state) {
  // Send it!
  $form_state['storage']['send_profile']->send();

  // Redirect to the page specified by the handler or to the front page.
  if (!isset($form_state['redirect'])) {
    if (!$url = $form_state['storage']['send_profile']->redirect()) $url = '<front>';
    $form_state['redirect'] = $url;
  }

  // Unset some stuff that would cause the form to rebuild.
  unset($form_state['rebuild'], $form_state['storage']);
}

/**
 * A submit handler that doesn't actually submit the send request.  Useful
 * for AHAH callbacks and buttons that change the form's state or appearance.
 */
function send_submit_refresh($form, &$form_state) {
  // $form_state['storage']['send'] = $form_state['values']['send'];
  $form_state['rebuild'] = TRUE;
  $form_state['redirect'] = FALSE;
}

/**
 * The #process handler for the main "send" element.
 */
function send_element_process($element, $edit, &$form_state, &$form) {
  if (!isset($form_state['storage']['send_profile'])) {
    $form_state['storage']['send_profile'] = send_profile_load($element['#send_profile']);
  }
  else {
    // Call this to ensure that the handler is loaded.
    $profile = send_profile_load((array)$form_state['storage']['send_profile']);
    $form_state['storage']['send_profile'] = $profile;
  }
  if ($edit) {
    $form_state['storage']['send_profile']->setValues($edit);
  }

  // Shorthand.
  $profile =& $form_state['storage']['send_profile'];

  $form['#action'] = $profile->action;

  // Send componenet elements may change their working values to be
  // compatible with send_profile attributes. This function sets those values.
  $element['#after_build'][] = 'send_element_after_build';

  // Make sure that the send_submit handler is processed for this form.
  if (!isset($form_state['submit_handlers']) || !$form_state['submit_handlers']) {
    $form_state['submit_handlers'] = array();
  }
  if (!in_array('send_submit', $form_state['submit_handlers'])) {
    $form_state['submit_handlers'][] = 'send_submit';
  }

  // Make sure that the send_validate handler is processed for this form.
  if (!isset($form_state['validate_handlers']) || !$form_state['validate_handlers']) {
    $form_state['validate_handlers'] = array();
  }
  if (!in_array('send_validate', $form_state['validate_handlers'])) {
    $form_state['validate_handlers'][] = 'send_validate';
  }

  // Force this group of values into $form_state['values']['send'].
  $element['#parents'] = array('send');
  $element['#tree'] = TRUE;

  // Sender element, based on administratively-configured element type.
  $element['sender'] = array(
    '#type' => $profile->from_element,
  );

  // Recipient element, based on administratively-configured element type.
  $element['recipients'] = array(
    '#type' => $profile->to_element,
    '#required' => TRUE,
  );

  // "Content" element, based on administratively-configured element type.
  $element['content'] = array(
    '#type' => $profile->message_element,
  );

  // Maintain a list of nodes for this message, stored as a value field.
  $element['nids'] = array(
    '#type' => 'value',
    '#value' => $profile->nids,
  );

  // And we're done here! These elements' #process callbacks take it from here.
  return $element;
}

/**
 * An #after_build callback for the send element.
 *
 * The elements used for different send form components may require working
 * values that are different from what the send profile actually uses for
 * processing.  In these cases, a form_type_hook_value for those elements is
 * expected to set things straight, and this function will save the adjusted
 * values back to the send profile.
 */
function send_element_after_build($element, &$form_state) {
  if ($values = $form_state['values']['send']) {
    $form_state['storage']['send_profile']->setValues($values);
  }
  return $element;
}

/**
 * An AHAH callback for the send element.
 *
 * This allows the composition interface to be re-processed for form changes
 * such as additional subscribers, mailing lists, content changes, etc.
 */
function send_form_js($element = NULL, $build_type = NULL) {
//TODO
module_load_include('form.inc', 'news', 'includes/news');
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  if ($build_type) $form_state['send_build_type'] = $build_type;

  $form_build_id = $_POST['form_build_id'];
  // Step #4.
  $form = form_get_cache($form_build_id, $form_state);

  // Preparing for #5.
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  $form['#programmed'] = $form['#redirect'] = FALSE;

  // Step #5.
  drupal_process_form($form_id, $form, $form_state);

  // Step #6 and #7 and #8.
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

  // I was having problems selectively rendering a sub-part to send, so I'm
  // just going to unset anything that's not the element we need.
  foreach (element_children($form['send']) as $key) {
    if ($key != $element) unset($form['send'][$key]);
  }
  unset($form['send']['#prefix'], $form['send']['#suffix']);
  unset($form['send'][$element]['#prefix'], $form['send'][$element]['#suffix']);

  // Clear out the form_errors array.  We're not here for validation.
  form_set_error(NULL, '', TRUE);
  drupal_get_messages('error', TRUE);

  $output = theme('status_messages') . drupal_render($form['send']);

  // Final rendering callback.
  drupal_json(array('status' => TRUE, 'data' => $output));
}

/**
 * An implementation of form_type_hook_value for send_template_select elements.
 */
function form_type_send_message_template(&$element, $edit = array()) {
  return array( $edit['template'] );
}

/**
 * A #process handler for a send_contact element.
 */
function send_contact_element_process($element, $edit, &$form_state, $form) {
  $field = send_form_get_field($element);
  $profile =& $form_state['storage']['send_profile'];

  $value = $profile->$field;
  if (!isset($value['mail'])) $value['mail'] = '';
  if (!isset($value['first_name'])) $value['first_name'] = '';
  if (!isset($value['last_name'])) $value['last_name'] = '';

  $element['#element_validate'] = array('send_contact_element_validate');
  $element['mail'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Email'),
    '#default_value' => $value['mail'],
    '#required'      => TRUE,
    '#size'          => 30,
    '#maxsize'       => 100,
  );
  $element['first_name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('First Name'),
    '#default_value' => $value['first_name'],
    '#size'          => 30,
    '#maxsize'       => 100,
  );
  $element['last_name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Last Name'),
    '#default_value' => $value['last_name'],
    '#size'          => 30,
    '#maxsize'       => 100,
  );

  return $element;
}

/**
 * An implementation of form_type_hook_value for send_contact elements.
 */
function form_type_send_contact_value(&$element, $edit = array()) {
  $contact = array();
  if (isset($edit['mail'])) {
    $contact['mail'] = $edit['mail'];
  }
  if (isset($edit['first_name']) || isset($edit['last_name'])) {
    $contact['name'] = trim($edit['first_name'] .' '. $edit['last_name']);
  }
  return $contact;
}

/**
 * An #element_validate handler for a send_contact element.
 */
function send_contact_element_validate($element, &$form_state) {
  if (!valid_email_address($element['#value']['mail'])) {
    form_set_error($element['#name'], t('Invalid email address'));
  }
}

/**
 * A #process handler for a send_user element.
 */
function send_user_element_process($element, $edit, &$form_state, $form) {
  $element['#required'] = FALSE;
  $element['name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('User name'),
    '#size'          => 15,
    '#maxsize'       => 100,
    '#autocomplete_path' => '',
  );

  // Special handling: "from" elements, you shouldn't be impersonating.
  if ($element['#name'] == 'sender') {
    global $user;
    $element['#value']['name'] = $user->name;
    $element['#access'] = FALSE;
  }
  return $element;
}

/**
 * An implementation of form_type_hook_value for send_user elements.
 *
 * Set the element value in a format that will be recognized by send_contact.
 */
function form_type_send_user_value(&$element, $edit = array()) {
  if (isset($element['#value'])) {
    $account = user_load(array('name' => $element['#value']['name']));
    return array(
      'uid' => $account->uid,
      'mail' => $account->mail,
      'name' => strip_tags(theme('username', $account)),
    );
  }
}

/**
 * An #element_validate handler for a send_user element.
 */
function send_user_element_validate($element, &$form_state) {
}

/**
 * A #process handler for a send_multiple element.
 */
function send_multiple_element_process($element, $edit, &$form_state, $form) {
  $profile =& $form_state['storage']['send_profile'];
  $field = send_form_get_field($element);

  $element['addresses'] = array(
    '#element_validate' => array('send_multiple_element_validate'),
    '#type'          => 'textarea',
    '#title'         => t('Email addresses'),
    '#default_value' => join(',', $profile->$field),
  );
  return $element;
}

/**
 * An #element_validate handler for a send_contact element.
 */
function send_multiple_element_validate($element, &$form_state) {
}

/**
 * An implementation of form_type_hook_value for send_multiple elements.
 *
 * Set the element value in a format that will be recognized by send_contact.
 */
function form_type_send_multiple_value(&$element, $edit = array()) {
  $recipients = array();

  if (!isset($edit['addresses'])) return;

  // Recipients are from a comma/line-delimited textarea.
  if ($candidates = preg_split('/[\r\n,]+/m', $edit['addresses'], -1, PREG_SPLIT_DELIM_CAPTURE)) {

    foreach ($candidates as $row) {
      $r = array();

      // Parse out the mail address.
      if (preg_match('/[A-Z0-9._%-]+@[A-Z0-9.-]+/i', $row, $m)) {
        $r['mail'] = $m[0];

        // "Name" is whatever is left.
        $r['name'] = trim(str_replace($r['mail'], '', $row), " ,;<>");

        // If possible, parse out the first name / last name components.
        $name_parts = preg_split('/\s+/', $r['name']);

        // Set first/last names, grouping any additional name parts into last.
        $r['first_name'] = array_shift($name_parts);
        $r['last_name'] = join(' ', $name_parts);
      }

      // Add this recipient to the list of recipients.
      if ($r['mail']) $recipients[] = $r;
    }
  }

  return $recipients;
}

/**
 * A #process handler for a send_message_compose element.
 */
function send_message_compose_element_process($element, $edit, &$form_state, $form) {
  $profile =& $form_state['storage']['send_profile'];

  if (isset($form_state['send_build_type'])) {
    $element['#send_build_type'] = $form_state['send_build_type'];
  }

  $element['#theme'] = 'send_message_compose_element';
  $element['#required'] = FALSE;

  //if ($edit) $profile->message = $edit;

  if (isset($profile->build_mode)) {
    $build_mode = $profile->build_mode;
  }
  else {
    if (!$build_mode = $profile->template()->build_mode) {
      $build_mode = 'teaser';
    }
  }

  if ($profile->compose_advanced && (!isset($element['#send_build_type']) || $element['#send_build_type'] != 'items')) {
    $element['admin'] = array(
      '#prefix' => '<div id="message-admin">',
      '#suffix' => '</div>',
    );

    if (count($profile->templates()) > 1) {
      $element['admin']['template'] = array(
        '#type'          => 'select',
        '#title'         => t('Message template'),
        '#options'       => $profile->templates(),
        '#default_value' => $profile->template()->stid,
        '#parents' => array('send', 'template'),
      );

      $id = 'edit-send-template-submit';
      $element['admin']['template_submit'] = array(
        '#id'            => $id,
        '#type'          => 'submit',
        '#value'         => t('Change template'),
        '#submit'        => array('send_submit_refresh'),
        '#ahah' => array(
          'path' => 'send/js/send_form/content',
          'wrapper' => 'send-message',
          'method' => 'replace',
          'effect' => 'fade',
        ),
      );

      // Add an auto-submit action to the template selection.
      $js = 'Drupal.behaviors.sendMessageForm = function(context) {
        $("#'. $id .'").hide();
        $("#edit-send-template").change( function() {
          $("#'. $id .'").click();
        });
      }';
      drupal_add_js($js, 'inline');
    }

    $element['admin']['build_mode'] = array(
      '#type' => 'select',
      '#title' => t('Display nodes as'),
      '#options' => $profile->buildModes(),
      '#default_value' => $build_mode,
      '#access' => $profile->editable('build_mode') && $profile->nids,
      '#parents' => array('send', 'build_mode'),
    );

    if ($element['admin']['build_mode']['#access']) {
      $id = 'send-compose-submit';
      $element['admin']['compose_submit'] = array(
        '#id'            => $id,
        '#type'          => 'submit',
        '#value'         => t('Update'),
        '#submit'        => array('send_submit_refresh'),
        '#ahah' => array(
          'path' => 'send/js/send_form/content/items',
          'wrapper' => 'message-content',
          'method' => 'replace',
          'effect' => 'fade',
        ),
      );

      // Add an auto-submit action to the admin form.
      $js = 'Drupal.behaviors.sendMessageAdminForm = function(context) {
        $("#'. $id .'").hide();
        $("#edit-send-build-mode").change( function() {
          $("#'. $id .'").click();
        });
      }';
      drupal_add_js($js, 'inline');
    }
  }

  if ($profile->editable('subject')) {
    $element['subject'] = array(
      '#type' => 'textfield',
      '#title' => t('Subject'),
      '#default_value' => $profile->subject(),
      '#parents' => array('send', 'subject'),
    );
  }
  else {
    $element['subject_display'] = array(
      '#type' => 'markup',
      '#value' => $profile->subject(),
      '#prefix' => '<h3>',
      '#suffix' => '</h3>',
      '#parents' => array('send', 'subject_display'),
    );
    $element['subject'] = array(
      '#type' => 'value',
      '#value' => $profile->subject(),
      '#parents' => array('send', 'subject'),
    );
  }

  // Respect the order that was sent through by table-dragging code.
  foreach ($profile->content() as $key => $val) {
    $element['items'][$key] = array();
  }

  $element['items']['message']['content'] = array(
    '#type'  => 'textarea',
    '#title' => t('Message'),
    '#default_value' => $profile->content['message']['content'],
    '#parents' => array('send', 'content', 'message', 'content'),
  );
  $element['items']['message']['format'] = filter_form($profile->body_format, NULL, array('send', 'content', 'message', 'format'));
  $element['items']['message']['format']['#access'] = $profile->editable('body_format');

  foreach ($profile->nids as $nid) {
    $node = node_load($nid);
    $node->build_mode = $build_mode;
    $teaser = $node->build_mode == 'teaser';
    $content = node_view($node, $teaser);
    $element['items']['node-'. $nid] = array(
      '#type' => 'markup',
      '#value' => $content,
      'nid' => array(
        '#type' => 'value',
        '#value' => $nid,
        '#parents' => array('send', 'content', 'node-'. $nid, 'nid' ),
      ),
    );
  }

  if ($profile->compose_advanced && $profile->editable('body')) {
    foreach ($element['items'] as $key => $info) {
      $element['items'][$key]['weight'] = array(
        '#type' => 'hidden',
        '#parents' => array('send', 'content', $key, 'weight' ),
      );
    }
    if (count($element['items']) > 1) {
      $element['items']['#theme'] = 'send_draggable';
    }
  }

  return $element;
}

/**
 * A #process handler for a send_message_edit element.
 */
function send_message_edit_element_process($element, $edit, &$form_state, $form) {
  $profile =& $form_state['storage']['send_profile'];

  if ($profile->editable('subject')) {
    $element['subject'] = array(
      '#type' => 'textfield',
      '#title' => t('Subject'),
      '#default_value' => $profile->subject(),
      '#parents' => array('send', 'subject'),
    );
  }
  else {
    $element['subject_display'] = array(
      '#type' => 'markup',
      '#value' => $profile->subject(),
      '#prefix' => '<h3>',
      '#suffix' => '</h3>',
      '#parents' => array('send', 'subject_display'),
    );
    $element['subject'] = array(
      '#type' => 'value',
      '#value' => $profile->subject(),
      '#parents' => array('send', 'subject'),
    );
  }

  $element['body']['body'] = array(
    '#type' => 'textarea',
    '#default_value' => $profile->body(),
    '#rows' => 30,

    // Use 'body' rather than 'content' to avoid overriting $profile->content().
    '#parents' => array('send', 'body', 'body'),
  );

  $element['body']['format'] = filter_form($profile->body_format, NULL, array('send', 'body', 'format'));
  $element['body']['format']['#access'] = $profile->editable('body_format');
  return $element;
}

/**
 * A helper function to determine the name of the field for 
 * a form element. Called from within a send element form.  It's most 
 * useful on contact elements, where you want to determine whether 
 * you're a sender/recipient element.
 */
function send_form_get_field($element) {
  $field  = array_pop($element['#parents']);
  return $field;
}
