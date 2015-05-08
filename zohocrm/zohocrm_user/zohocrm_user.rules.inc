<?php

/**
 * @file
 * Provides basic rules module support.
 */

/**
 * Implements hook_rules_action_info().
 */
function zohocrm_user_rules_action_info() {
  $info = array();

  $defaults = array(
    'parameter' => array(
      'user' => array(
        'type' => 'user',
        'label' => t('User'),
      ),
      'mapping' => array(
        'type' => 'text',
        'label' => t('Drupal to Zoho Mapping'),
        'options list' => 'zohocrm_mappings_select_options',
      ),
    ),
    'group' => t('Zoho CRM'),
  );

  $info['zohocrm_user_rules_action_send_user_data'] = $defaults + array(
    'label' => t('Send user account data to Zoho'),
  );

  $info['zohocrm_user_rules_action_delete_zoho_record'] = $defaults + array(
    'label' => t('Delete Zoho CRM record'),
  );

  return $info;
}

/**
 * Action callback for sending user data to Zoho CRM.
 */
function zohocrm_user_rules_action_send_user_data($user, $mapping) {
  $settings = array('mapping' => $mapping);
  zohocrm_send_data($user, $settings);
}

/**
 * Action callback for deleting zoho record.
 */
function zohocrm_user_rules_action_delete_zoho_record($user, $mapping) {
  module_load_include('inc', 'zohocrm', 'zohocrm.rules');
  $settings = array('mapping' => $mapping);
  zohocrm_action_delete_zoho_record($user, $settings);
}
