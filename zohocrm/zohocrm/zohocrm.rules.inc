<?php

/**
 * @file
 * Provides basic Rules module support.
 */

/**
 * Wrapper around zohocrm_delete_zoho_record().
 *
 * Used by Rules' actions for deleting Zoho records.
 *
 * @param object $object
 *   Drupal object on which action is being preformed
 *   (e.g. node, user, etc).
 * @param array $settings
 *   Rules' action settings.
 */
function zohocrm_action_delete_zoho_record($object, $settings) {
  if (zohocrm_is_receiving_zoho_data() === TRUE) {
    // This action was triggered by Zoho data being saved to Drupal.
    // No need to loop back to Zoho again.
    return;
  }

  $mapping = zohocrm_mapping_load($settings['mapping']);
  if (empty($mapping['status'])) {
    return;
  }

  $drupal_id = zohocrm_drupal_id_from_object($object, $mapping);
  $zoho_id = zohocrm_record_mapping_get_zoho_id($mapping['drupal_endpoint'], $mapping['zoho_module'], $drupal_id);

  if (!empty($zoho_id)) {
    zohocrm_delete_zoho_record($zoho_id, $mapping['zoho_module']);
  }
}
