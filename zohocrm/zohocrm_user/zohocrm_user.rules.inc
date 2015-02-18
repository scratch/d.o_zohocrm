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

  $info['user_action_send_user_data'] = array(
    'label' => t('Zuser: Send user account data to Zoho'),
    'parameter' => array(
      'user' => array(
        'type' => 'user',
        'label' => t('User'),
      ),
    ),
    'module' => 'Zoho CRM',
    'group' => 'Zoho',
     'base' => '',
    'access callback' => 'zohocrm_rules_action_send_user_data',
  );

  $info['user_action_delete_zoho_record'] = array(
    'label' => t('Zuser: Delete Zoho CRM record'),
    'parameter' => array(
      'node' => array(
        'type' => 'user',
        'label' => t('User'),
      ),
    ),
    'group' => 'Zoho',
     'base' => '',
    'access callback' => 'zohocrm_user_rules_action_delete_zoho_record',
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
