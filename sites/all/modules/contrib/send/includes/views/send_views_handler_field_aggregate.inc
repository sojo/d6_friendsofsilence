<?php

/**
 * @file
 *
 * A Views handler that provides count summaries for send activity.
 */

class send_views_handler_field_aggregate extends views_handler_field_numeric {

  function query() {
    // The table containing the values (e.g. send_recipient) set in definition.
    $count_table = $this->query->ensure_table($this->definition['table']);
    $this->table_alias = $count_table;

    // The field to aggregate (e.g. contact_count), also set in definition.
    $count_field = $this->definition['table'] . '.' . $this->definition['field'];

    $field = 'SUM(' . $count_field . ')';
    $alias = $this->definition['table'] . '_' . $this->definition['field'] . '_sum';

    $this->field_alias = $this->query->add_field('', $field, $alias, array('aggregate' => TRUE));
  }
}
