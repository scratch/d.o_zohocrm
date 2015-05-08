<?php

/**
 * @file
 * Provides basic rules module support.
 */

/**
 * Implements hook_rules_action_info().
 */
function zohocrm_node_rules_action_info() {
  $defaults = array(
   'parameter' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Node'),
        'description' => t('Znode: send node data to Zoho.'),
        'save' => TRUE,
      ),
    ),
    'group' => t('Zoho'),
  );

  $actions['node_action_send_node_data'] = $defaults + array(
    'label' => t('Znode: Send node data to Zoho'),
    'base' => '',
    'callback' => 'zohocrm_rules_action_send_node_data',
  );

  $actions['node_action_delete_node_data'] = $defaults + array(
    'label' => t('Znode: Delete zoho node'),
    'base' => '',
    'callback' => 'zohocrm_node_rules_action_delete_zoho_record',
  );

  return $actions;
}



/*
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
*/

function _zohocrm_rules_action_send_node_data($node, $settings) {
  zohocrm_send_data($node, $settings);
}

function zohocrm_node_rules_action_delete_zoho_record($node, $settings) {
  zohocrm_action_delete_zoho_record($node, $settings);
}
