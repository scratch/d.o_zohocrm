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

  $info['zohocrm_rules_action_send_user_data'] = array(
    'label' => t('Send user account data to Zoho'),
    'arguments' => array(
      'user' => array(
        'type' => 'user',
        'label' => t('User'),
      ),
    ),
    'help' => t('Help text.'),
    'module' => 'Zoho CRM',
  );

  $info['zohocrm_user_rules_action_delete_zoho_record'] = array(
    'label' => t('Delete Zoho CRM record'),
    'arguments' => array(
      'node' => array(
        'type' => 'user',
        'label' => t('User'),
      ),
    ),
    'help' => t(''),
    'module' => 'Zoho CRM',
  );

  return $info;
}

function zohocrm_rules_action_send_user_data($user, $settings) {
  zohocrm_send_data($user, $settings);
}

function zohocrm_rules_action_send_user_data_form($settings, &$form) {
  zohocrm_rules_action_form($settings, $form);
}

function zohocrm_user_rules_action_delete_zoho_record($user, $settings) {
  zohocrm_action_delete_zoho_record($user, $settings);
}

function zohocrm_user_rules_action_delete_zoho_record_form($settings, &$form) {
  zohocrm_rules_action_form($settings, $form);
}
