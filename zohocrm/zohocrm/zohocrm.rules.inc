<?php

/**
 * @file
 * Provides basic rules module support.
 */

/**
 * Settings form for Rules' actions.
 */
function zohocrm_rules_action_form($settings, &$form) {
  $settings += array('mapping' => '');

  $options = zohocrm_mappings_select_options();
  $form['settings']['mapping'] = array(
    '#type' => 'select',
    '#title' => t('Drupal to Zoho Mapping'),
    '#default_value' => $settings['mapping'],
    '#options' => $options,
    '#required' => TRUE,
    '#disabled' => empty($options),
    '#description' => empty($options) ? t('There are no Zoho CRM mappings defined.') : '',
  );
}

/**
 * Wrapper around zohocrm_delete_zoho_record(). Used by Rules'
 * action and Ubercart's CA action for deleting Zoho records.
 *
 * @param $object
 *   Drupal object on which action is being preformed
 *   (e.g. node, user, ubercart order).
 * @param $settings
 *   Action settings array.
 *
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
