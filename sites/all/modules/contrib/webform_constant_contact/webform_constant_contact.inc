<?php
// $Id$

/**
 * @file: Webform components to integrate the constant contact module and webform
 * So you can subscribe to a constant contact mailing list using a webform generated form
 */

/**
 * Create a default cc_email component.
 */
function _webform_defaults_cc_email()
{
    return array(
        'name' => '',
        'form_key' => NULL,
        'mandatory' => 0,
        'pid' => 0,
        'weight' => 0,
        'user_email' => 1,
        'extra' => array(
            'options' => '',
            'questions' => '',
            'optrand' => 0,
            'qrand' => 0,
            'description' => '',
        ),
    );
}

/**
 * Implementation of _webform_theme_component().
 */
function _webform_theme_cc_email()
{
    return array(
        'webform_cc_email' => array(
            'arguments' => array('element' => NULL),
        ),
        'webform_display_cc_email' => array(
            'arguments' => array('component' => NULL, 'value' => NULL, 'format' => 'plain'),
        ),
    );
}

/**
 * Implementation of _webform_edit_component().
 */
function _webform_edit_cc_email($component)
{
    if (! module_exists('constant_contact'))
    {
        $text = 'This form uses the "cc_email" component which requires the !link to be installed and enabled!';
        $link = l('"Constant Contact" module', 'http://drupal.org/project/constant_contact', array('attributes' => array('target' => '_blank')));
        $replace = array(
            '!link' => $link,
        );
        form_set_error('', t($text, $replace));
    }


    // get the constant contact contact lists
	$lists = array();

	// if we have an object get the users contact lists
	$cc = constant_contact_create_object();

	if(is_object($cc)):
		$lists = constant_contact_get_lists($cc);
	endif;

    $form['user_email'] = array(
        '#type' => 'checkbox',
        '#title' => t('Prefill Email Field if User Logged in'),
        '#default_value' => $component['value'] == '%useremail' ? 1 : 0,
        '#description' => t('If a user is logged in the email field will be prefilled with their email address.'),
        '#weight' => 0,
        '#element_validate' => array('_webform_edit_cc_email_validate'),
    );
    $form['extra']['action'] = array(
        '#type'          => 'radios',
        '#title'         => t('Form Action'),
        '#default_value' => (trim($component['extra']['action']) == '') ? 'subscribe' : $component['extra']['action'],
        '#description'   => t('Choose the action to be taken on submit.'),
        '#required'      => TRUE,
        '#options'       => array(
            'subscribe'        => t('Subscription'),
            'unsubscribe'      => t('Unsubscription'),
    )
    );
    $form['extra']['field_size'] = array(
        '#type' => 'textfield',
        '#title' => t('Size of the Email Field'),
        '#default_value' => $component['extra']['field_size'],
        '#description' => t('Specify the size of the email field, numbers only.'),
    );

    $form['extra']['subscribe_checkbox'] = array(
        '#type'          => 'radios',
        '#title'         => t('Display Explicit Subscribe Checkbox'),
        '#default_value' => (trim($component['extra']['subscribe_checkbox']) == '') ? 1 : $component['extra']['subscribe_checkbox'],
        '#description'   => t('If you would like to display a subscribe checkbox set this option to yes. if you do not display a subscribe checkbox users will have to select a contact list to subscribe, if your not displaying contact lists then whether or not the email field is required will determine if the user is subscribed, i.e. if email field is not required and user enters no email their not subscribed and if they do enter one they are subscribed.'),
        '#options'       => array(
            1        => t('Yes'),
            0        => t('No'),
        )
    );
    $form['extra']['subscribe_checkbox_title'] = array(
        '#type' => 'textfield',
        '#title' => t('Subscribe Checkbox Title'),
        '#default_value' => $component['extra']['subscribe_checkbox_title'],
        '#description' => t('A title for the subscribe checkbox.'),
    );
    $form['extra']['default_optin'] = array(
        '#type'          => 'radios',
        '#title'         => t('Optin Users by Default'),
        '#default_value' => (trim($component['extra']['default_optin']) == '') ? 1 : $component['extra']['default_optin'],
        '#description'   => t('This determines if the subscribe checkbox is checked by default.'),
        '#options'       => array(
            1        => t('Yes'),
            0        => t('No'),
        )
    );
    $form['extra']['show_contact_list_selection'] = array(
        '#type' => 'checkbox',
        '#title' => t('Show a Contact List Selection to Users'),
        '#default_value' => $component['extra']['show_contact_list_selection'],
        '#description' => t('If this box is checked a contact list selection will be shown to users.'),
    );
    $form['extra']['contact_list_selection_mandatory'] = array(
        '#type' => 'checkbox',
        '#title' => t('Contact List Selection Mandatory'),
        '#default_value' => $component['extra']['contact_list_selection_mandatory'],
        '#description' => t('Check this option if the user must choose at least one contact list.'),
    );
    $form['extra']['list_selection_format'] = array(
        '#type'          => 'radios',
        '#title'         => t('Contact List Selection Format'),
        '#default_value' => (trim($component['extra']['list_selection_format']) == '') ? 'checkboxes' : $component['extra']['list_selection_format'],
        '#description'   => t('Choose what format of contact list selection the user will be presented with.'),
        '#required'      => TRUE,
        '#options'       => array(
            'checkboxes'        => t('Checkboxes'),
            'select'            => t('Multi-Select List'),
        )
    );
    $form['extra']['contact_list_selection_title'] = array(
        '#type' => 'textfield',
        '#title' => t('Contact List Selection Title'),
        '#default_value' => $component['extra']['contact_list_selection_title'],
        '#description' => t('An optional title for the contact list selection field, this is required if setting the "Contact list selection mandatory" option on. Set to a blank string to remove it.'),
    );
    $form['extra']['lists'] = array(
        '#type'          => 'select',
        '#title'         => t("Contact Lists"),
        '#default_value' => (is_array($component['extra']['lists'])) ? $component['extra']['lists'] : array(),
        '#description'   => t('If you show the contact list selection, choose which contact lists will be available to select, if you do not show the contact list selection choose which lists their automatically added to.<br />If you do not select any lists here all lists will be used for both situations.'),
        '#options'       => $lists,
        '#multiple' => true,
        '#size' => 10,
    );
    return $form;
}

/**
 * Validation function for the email edit form.
 */
function _webform_edit_cc_email_validate($element, &$form_state)
{
    if ($form_state['values']['user_email'])
    {
        $form_state['values']['value'] = '%useremail';
    }
}

/**
 * Implementation of _webform_render_component().
 */
function _webform_render_cc_email($component, $value = NULL)
{
    if($value[0])
    {
        $default_value = $value[0];
    }
    else
    {
        $default_value = _webform_filter_values($component['value']);
    }
    $element = array(
        '#weight'            => $component['weight'],
        '#prefix'            => '<div class="webform-component-'. $component['type'] .'" id="webform-component-'. $component['form_key'] .'">',
        '#suffix'            => '</div>',
        '#webform_component' => $component,
        '#theme'             => 'webform_cc_email',
        '#element_validate'  => array('_webform_validate_cc_email'),
    );
    $element['cc_email_address'] = array(
        '#required'          => $component['mandatory'],
        '#title'             => htmlspecialchars($component['name'], ENT_QUOTES),
        '#description'             => htmlspecialchars($component['extra']['description'], ENT_QUOTES),
        '#type'              => 'textfield',
        '#default_value'     => $default_value,
        '#field_prefix'      => empty($component['extra']['field_prefix']) ? NULL : check_plain($component['extra']['field_prefix']),
        '#field_suffix'      => empty($component['extra']['field_suffix']) ? NULL : check_plain($component['extra']['field_suffix']),
    );

    if(isset($component['extra']['attributes'])):
        $element['cc_email_address']['#attributes'] = $component['extra']['attributes'];
    endif;

    if(trim($component['extra']['field_size']) != '' AND is_numeric($component['extra']['field_size'])):
        $element['cc_email_address']['#size'] = $component['extra']['field_size'];
    endif;

    // build the contact lists based on the options they have selected    // get the constant contact contact lists
	$lists = array();

	// if we have an object get the users contact lists
	$cc = constant_contact_create_object();

	if(is_object($cc)):
		$lists = constant_contact_get_lists($cc);
	endif;
    $selected_lists = (is_array($component['extra']['lists'])) ? $component['extra']['lists'] : array();

    if($component['extra']['action'] == 'subscribe' AND $component['extra']['subscribe_checkbox']):
        $element['subscribe'] = array(
            '#title' => (!empty($component['extra']['subscribe_checkbox_title'])) ? $component['extra']['subscribe_checkbox_title'] : '',
            '#type' => 'checkbox',
            '#default_value' => $component['extra']['default_optin'],
        );
    endif;

    if ($component['extra']['show_contact_list_selection']):
        $options = array();

        if(count($selected_lists) > 0):
            foreach($selected_lists as $list):
                if($list > 0 AND isset($lists[$list])):
                    $options[$list] = $lists[$list];
                endif;
            endforeach;

            if(!count($options)):
                $options = $lists;
            endif;
        else:
            $options = $lists;
        endif;

        $element['list_selection'] = array(
            '#title' => (!empty($component['extra']['contact_list_selection_title'])) ? $component['extra']['contact_list_selection_title'] : '',
            '#prefix' => '',
            '#type' => $component['extra']['list_selection_format'],
            '#default_value' => array(),
            '#options' => $options,
            '#required' => $component['extra']['contact_list_selection_mandatory'],
        );

        if($component['extra']['list_selection_format'] == 'select'):
            $element['list_selection']['#multiple'] = true;
            $element['list_selection']['#size'] = 10;
        endif;
    endif;

    return $element;
}


/**
 * A Drupal Forms API Validation function. Validates the entered values from
 * email components on the client-side form.
 * @param $form_element
 *   The e-mail form element.
 * @param $form_state
 *   The full form state for the webform.
 * @return
 *   None. Calls a form_set_error if the e-mail is not valid.
 */
function _webform_validate_cc_email($form_element, $form_state)
{
    if (! module_exists('constant_contact'))
    {
        $text = 'This form uses the "cc_email" component which requires the !link to be installed and enabled!';
        $link = l('"Constant Contact" module', 'http://drupal.org/project/constant_contact', array('attributes' => array('target' => '_blank')));
        $replace = array(
            '!link' => $link,
        );
        form_set_error('', t($text, $replace));
    }

    $component = $form_element['#webform_component'];
    if (!empty($form_element['cc_email_address']['#value']) && !valid_email_address($form_element['cc_email_address']['#value']))
    {
        form_error($form_element, t('%value is not a valid email address.', array('%value' => $form_element['cc_email_address']['#value'])));
    }
    elseif (empty($form_element['cc_email_address']['#value']) && $form_element['#required'])
    {
        form_error($form_element, t('E-mail address is required.', array('%name' => $component['name'])));
    }
}

/**
 * Implementation of _webform_submit_component().
 */
function _webform_submit_cc_email($component, $value)
{
    global $user;
    $mail = $value['cc_email_address'];
    $return_val = array(0 => $mail);
    
    // make sure we have a valid email address before continuing
    if(!valid_email_address($mail)):
        return false;
    elseif(isset($value['subscribe']) AND !intval($value['subscribe'])):
        return $return_val;
    endif;

    // get CC object
    $cc = constant_contact_create_object();
    if(!is_object($cc)):
        return false;
    endif;

	$lists = constant_contact_get_lists($cc);
    $contact_lists = array();

    if ($component['extra']['show_contact_list_selection']):
        if(is_array($value['list_selection']) AND count($value['list_selection']) > 0):
            foreach($value['list_selection'] as $list):
                if($list > 0 AND isset($lists[$list])):
                    $contact_lists[$list] = $lists[$list];
                endif;
            endforeach;
        endif;
    else:
        $contact_lists = (is_array($component['extra']['lists'])) ? array_keys($component['extra']['lists']): array();
    endif;

    if(count($contact_lists) > 0):
        $cc->set_action_type('contact'); /* important, tell CC that the contact made this action */
        $contact = $cc->query_contacts($mail);

        $return_val[2] = serialize($contact_lists);

        if($component['extra']['action'] == 'subscribe'):;
            $return_val[1] = 'Subscribe';

            // subscribe a user
            if($contact):
                // get the contacts full details
                $contact = $cc->get_contact($contact['id']);

                $contact_lists = (is_array($contact['lists'])) ? array_merge($contact['lists'], array_keys($contact_lists)) : array_keys($contact_lists);

                // contact exists so update their subscribed lists
                $status = $cc->update_contact($contact['id'], $mail, $contact_lists);
            else:
                // contact does not exist so subscribe them
                //$status = $cc->create_contact($mail, array_keys($contact_lists));
                dsm(array_keys($contact_lists));
            endif;
        else:
            $return_val[1] = 'Unsubscribe';

            // unsubscribe a user
            if($contact):
                $contact = $cc->get_contact($contact['id']);

                // remove them from the contact lists they have chosen
                foreach($contact_lists as $list_id => $list_name):
                    if(isset($contact['lists'][$list_id])):
                        unset($contact['lists'][$list_id]);
                    endif;
                endforeach;

                // contact exists so update their subscribed lists
                $status = $cc->update_contact($contact['id'], $mail, $contact['lists']);
            endif;
        endif;
	endif;

  return $return_val;
}


/**
 * Implementation of _webform_display_component().
 */
function _webform_display_cc_email($component, $value, $format = 'html')
{
    $values = array(
        array(
            '#title' => $component['name'],
            '#weight' => $component['weight'],
            '#theme' => 'webform_display_cc_email',
            '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
            '#post_render' => array('webform_element_wrapper'),
            '#component' => $component,
            '#format' => $format,
            '#value' => isset($value[0]) ? $value[0] : '',
        ),
    );

    // add the action
    if(isset($value[1])):
        $values[] = array(
            '#title' => 'Action',
            '#weight' => 998,
            '#theme' => 'webform_display_cc_email',
            '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
            '#post_render' => array('webform_element_wrapper'),
            '#component' => $component,
            '#format' => $format,
            '#value' => isset($value[1]) ? $value[1] : '',
        );
    endif;

    // add the contact lists
    if(isset($value[2])):
        $values[] = array(
            '#title' => 'Contact Lists',
            '#weight' => 999,
            '#theme' => 'webform_display_cc_email',
            '#theme_wrappers' => array('webform_element'),
            '#post_render' => array('webform_element_wrapper'),
            '#component' => $component,
            '#format' => 'html',
            '#value' => isset($value[2]) ? implode(', ', array_values(unserialize($value[2]))) : '',
        );
    endif;

    return $values;
}

/**
 * Theme a webform time element.
 */
function theme_webform_cc_email($element)
{
    if (form_get_error($element))
    {
        $element['cc_email_address']['#attributes']['class'] = 'error';
    }
    $output = '<div class="form-item">' . drupal_render($element) . '</div>';
    $element['#type'] = 'element';
    return theme('form_element', $element, $output);
}

/**
 * Format the text output for this component.
 */
function theme_webform_display_cc_email($element)
{
    $element['#value'] = empty($element['#value']) ? ' ' : $element['#value'];
    return $element['#format'] == 'html' ? check_plain($element['#value']) : $element['#value'];
}

/**
 * Implementation of _webform_analysis_component().
 */
function _webform_analysis_cc_email($component)
{
    $query = 'SELECT data '.
    ' FROM {webform_submitted_data} '.
    ' WHERE nid = %d '.
    ' AND cid = %d';
    $nonblanks = 0;
    $submissions = 0;
    $wordcount = 0;

    $result = db_query($query, $component['nid'], $component['cid']);
    while ($data = db_fetch_array($result))
    {
        if (strlen(trim($data['data'])) > 0 )
        {
            $nonblanks++;
            $wordcount += str_word_count(trim($data['data']));
        }
        $submissions++;
    }
    $rows[0] = array( t('Submissions'), $submissions);
    return $rows;
}


/**
 * Module specific instance of hook_help().
 *
 * This allows each Webform component to add information into hook_help().
 */
function _webform_help_component($section)
{
    switch ($section)
    {
        case 'admin/settings/webform#cc_email':
        return t('Allows creation of an email field and contact list selection when you have the constant contact module installed.');
    }
}


/**
 * Implementation of _webform_csv_headers_component().
 */
function _webform_csv_headers_cc_email($component, $export_options)
{
    $header = array();
    $header[0] = '';
    $header[1] = '';
    $header[2] = $component['name'];
    return $header;
}

/**
 * Implementation of _webform_csv_data_component().
 */
function _webform_csv_data_cc_email($component, $export_options, $value)
{
    return empty($value[0]) ? '' : $value[0];
}


/**
 * Implementation of _webform_table_component().
 */
function _webform_table_cc_email($component, $value)
{
    return check_plain(empty($value[0]) ? '' : $value[0]);
}

function webform_constant_contact_webform_submission_insert($node, $submission) {
	dsm($submission->data);
	if ($node->nid == 3506) {
		$first_name = $submission->data[12][value][0];
		$last_name =  $submission->data[13][value][0];
		$email =      $submission->data[10][value][0];
		$lists = array();
		$fields = array();
		$lists[0] = 13;
		$lists[1] = 10;
		$fields[FirstName] = $first_name;
		$fields[LastName] = $last_name;
		$fields[EmailType] = 'HTML';
		$cc = constant_contact_create_object();
		$contact = $cc->query_contacts($email);
		if (!$contact) {
			//$status = $cc->create_contact($email, $lists, $fields);
		}
 }
}
