<?php

/**
 * @file
 *
 * A Views handler that provides count summaries for send activity.
 */

class send_track_views_handler_field_aggregate extends send_views_handler_field_aggregate {

  function options(&$options) {
    parent::options($options);

    $options['unique'] = 1;
    $options['method'] = 'percentage';
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['unique'] = array(
      '#title' => t('Count unique activities only.'),
      '#type'  => 'checkbox',
      '#description'  => t('Recipients may have performed this activity multiple times on a message. For example, someone may have viewed an email more than once. This option limits results to only the first event. Recommended.'),
      '#default_value' => $this->options['unique'],
    );

    $form['method'] = array(
      '#title' => t('Calculation method'),
      '#type'  => 'radios',
      '#options' => array(
        'percentage' => t('Show the percentage of total recipients who performed this activity. For best results, choose the "unique" option above.'),
        'count' => t('Show the total number of times the activity occurred for a mailing.'),
      ),
      '#default_value' => $this->options['method'],
    );
  }

  function query() {
    // Use the 'unique' field if that option is specified.
    if ($this->options['unique']) {
      $this->definition['field'] .= '_unique';
    }

    // Customize handling for percentage-based results.
    if ($this->options['method'] == 'percentage') {

      // The table containing the values (e.g. send_recipient) set in definition.
      $count_table = $this->query->ensure_table($this->definition['table']);
      $this->table_alias = $count_table;
  
      // The field to aggregate (e.g. open), also set in definition.
      $count_field = $this->definition['table'] . '.' . $this->definition['field'];

      // The total results field (e.g. contact_count).
      $total_field = $this->definition['table'] . '.contact_count';
  
      $field = 'LEAST(100, 100 * (SUM(' . $count_field . ') / SUM(' . $total_field . ')))';
      $alias = $this->definition['table'] . '_' . $this->definition['field'] . '_percentage';
  
      $this->field_alias = $this->query->add_field('', $field, $alias, array('aggregate' => TRUE));

    }
    else {
      parent::query();
    }
  }
}
