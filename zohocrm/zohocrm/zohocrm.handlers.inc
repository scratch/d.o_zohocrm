<?php

/**
 * @file
 * Field handlers that convert Drupal field values into Zoho compatible format.
 *
 */

function zohocrm_field_handler_default($source_field, $destination_field, $data, $sync_direction, &$destination_object = NULL) {
  if (!empty($destination_object)) {
    $destination_object->{$destination_field} = $data->{$source_field};
  }
  return $data->{$source_field};
}

function zohocrm_field_handler_timestamp($source_field, $destination_field, $data, $sync_direction, &$destination_object = NULL) {
  $result = '';
  $value = $data->{$source_field};

  switch ($sync_direction) {

    case ZOHOCRM_SYNC_DRUPAL_TO_ZOHO:
      if (!empty($value) && is_numeric($value)) {
        $timezone_name = variable_get('zohocrm_timezone', '');
        $timezone = new DateTimeZone($timezone_name);
        $date = new DateTime("now", $timezone);
        $result = format_date($value, 'custom', ZOHOCRM_DATETIME_FORMAT, $timezone->getOffset($date));
      }
      break;

    case ZOHOCRM_SYNC_ZOHO_TO_DRUPAL:
      if (!empty($value)) {
        $timezone = new DateTimeZone('UTC');
        try {
          $date = new DateTime($value, $timezone);
          $result = $date->getTimestamp();
          $destination_object->{$destination_field} = $result;
        }
        catch (Exception $e) {
          watchdog('zohocrm', 'Invalid date value received from Zoho CRM: @value', array('@value' => $value), WATCHDOG_WARNING);
        }
      }
      break;
  }

  return $result;
}

//function zohocrm_field_handler_concatenate_dtz($fields, $object) {
//  $values = array();
//
//  foreach ($fields as $field) {
//    $value = trim($object->$field);
//    if (is_string($value) && $value != '') {
//      $values[] = $value;
//    }
//  }
//
//  return implode(' ', $values);
//}
