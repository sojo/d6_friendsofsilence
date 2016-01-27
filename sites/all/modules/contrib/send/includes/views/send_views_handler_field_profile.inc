<?php

/**
 * @file
 *
 * A Views handler that shows a profile by name.
 */

class send_views_handler_field_module extends views_handler_field {
  function render($values) {
    $value = $values->{$this->field_alias};
    $modules = send_profiles();
    return $modules[$value];
  }
}
