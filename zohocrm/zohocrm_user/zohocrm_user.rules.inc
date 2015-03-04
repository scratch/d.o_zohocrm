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

  $defaults = array (
    'parameter' => array (
      'user' => array(
        'type' => 'user',
        'label' => t('User'),
      ),
    ),
    'module' => 'zohocrm',
    'group' => 'Zoho',
    'base' => '',
  );


  $info['user_action_send_user_data'] = $defaults + array (
    'label' => t('Zuser: Send user account data to Zoho'),
    'callback' => 'zohocrm_rules_action_send_user_data',
  );

  $info['user_action_delete_zoho_record'] = $defaults + array (
    'label' => t('Zuser: Delete Zoho CRM record'),
    'callback' => 'zohocrm_user_rules_action_delete_zoho_record',
  );

  return $info;
}

function _zohocrm_rules_action_send_user_data($user, $settings) {
  zohocrm_send_data($user, $settings);
}

function _zohocrm_rules_action_send_user_data_form($settings, &$form) {
  zohocrm_rules_action_form($settings, $form);
}

function zohocrm_user_rules_action_delete_zoho_record($user, $settings) {
  zohocrm_action_delete_zoho_record($user, $settings);
}

function zohocrm_user_rules_action_delete_zoho_record_form($settings, &$form) {
  zohocrm_rules_action_form($settings, $form);
}
