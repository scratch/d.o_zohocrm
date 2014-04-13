<?php

/**
 * @file
 * Provides basic rules module support.
 */

/**
 * Implements hook_rules_action_info().
 */
function zohocrm_node_rules_action_info() {
  $info = array();

  $info['zohocrm_rules_action_send_node_data'] = array(
    'label' => t('Send node data to Zoho'),
    'arguments' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Node'),
      ),
    ),
    'help' => t('Help text.'),
    'module' => 'Zoho CRM',
  );

  $info['zohocrm_node_rules_action_delete_zoho_record'] = array(
    'label' => t('Delete Zoho CRM record'),
    'arguments' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Node'),
      ),
    ),
    'help' => t(''),
    'module' => 'Zoho CRM',
  );

  return $info;
}

function zohocrm_rules_action_send_node_data($node, $settings) {
  zohocrm_send_data($node, $settings);
}

function zohocrm_rules_action_send_node_data_form($settings, &$form) {
  zohocrm_rules_action_form($settings, $form);
}

function zohocrm_node_rules_action_delete_zoho_record($node, $settings) {
  zohocrm_action_delete_zoho_record($node, $settings);
}

function zohocrm_node_rules_action_delete_zoho_record_form($settings, &$form) {
  zohocrm_rules_action_form($settings, $form);
}
