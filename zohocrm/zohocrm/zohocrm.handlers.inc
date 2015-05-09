<?php

/**
 * @file
 * Defines common field handlers used by all zohocrm_* modules.
 *
 * Field handlers are used for conversion of field values between Drupal and
 * Zoho compatible formats.
 */

/**
 * Default field handler that just passes source value without any conversion.
 *
 * @param string $source_field
 *   Name of the source field.
 * @param string $destination_field
 *   Name of the destination field.
 * @param object $data
 *   Object containing source data.
 * @param int $sync_direction
 *   Direction of sending data (Drupal to Zoho or Zoho to Drupal).
 * @param object $destination_object
 *   Drupal object into which to save received Zoho data. Can be used only
 *   during Zoho to Drupal sync.
 *
 * @return mixed
 *   Value converted into the format understood by the destination end of
 *   synchronization.
 *   Note: This value is ignored during Zoho to Drupal sync. Instead it must be
 *   assigned to the appropriate property ($destination_field) of the
 *   destination object.
 */
function zohocrm_field_handler_default($source_field, $destination_field, $data, $sync_direction, &$destination_object = NULL) {
  if (!empty($destination_object)) {
    $destination_object->{$destination_field} = $data->{$source_field};
  }

  return $data->{$source_field};
}

/**
 * Converts between Drupal timestamp and Zoho DateTime format.
 *
 * @see zohocrm_field_handler_default()
 */
function zohocrm_field_handler_timestamp($source_field, $destination_field, $data, $sync_direction, &$destination_object = NULL) {
  $result = '';
  $value = $data->{$source_field};

  switch ($sync_direction) {

    case ZOHOCRM_SYNC_DRUPAL_TO_ZOHO:
      if (!empty($value) && is_numeric($value)) {
        $timezone = variable_get('zohocrm_timezone', NULL);
        $result = format_date($value, 'custom', ZOHOCRM_DATETIME_FORMAT, $timezone);
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
