<?php

/**
 * @file
 * Conditional Actions hooks and functions necessary
 * to make the zohocrm related actions work.
 */

/**
 * Implements hook_ca_action().
 */
function zohocrm_ubercart_ca_action() {
  $order_arg = array(
    '#entity' => 'uc_order',
    '#title' => t('Order'),
  );

  $actions['zohocrm_ubercart_send_order_data'] = array(
    '#title' => t('Send order data to Zoho CRM'),
    '#category' => t('Zoho CRM'),
    '#callback' => 'zohocrm_ubercart_action_send_order_data',
    '#arguments' => array(
      'order' => $order_arg,
    ),
  );

  $actions['zohocrm_ubercart_delete_zoho_record'] = array(
    '#title' => t('Delete Zoho CRM record'),
    '#category' => t('Zoho CRM'),
    '#callback' => 'zohocrm_ubercart_action_delte_zoho_record',
    '#arguments' => array(
      'order' => $order_arg,
    ),
  );

  return $actions;
}

function zohocrm_ubercart_action_send_order_data($order, $settings) {
  zohocrm_send_data($order, $settings);
}

function zohocrm_ubercart_action_send_order_data_form($form_state, $settings = array()) {
  return zohocrm_ubercart_ca_action_form($form_state, $settings);
}

function zohocrm_ubercart_action_delte_zoho_record($order, $settings) {
  module_load_include('inc', 'zohocrm', 'zohocrm.rules');
  zohocrm_action_delete_zoho_record($order, $settings);
}

function zohocrm_ubercart_action_delte_zoho_record_form($form_state, $settings = array()) {
  return zohocrm_ubercart_ca_action_form($form_state, $settings);
}

function zohocrm_ubercart_ca_action_form($form_state, $settings) {
  $form = array();

  $form['mapping'] = array(
    '#type' => 'select',
    '#title' => t('Drupal to Zoho Mapping'),
    '#default_value' => isset($settings['mapping']) ? $settings['mapping'] : '',
    '#options' => zohocrm_mappings_select_options(),
    '#description' => empty($options) ? t('There are no Zoho CRM mappings defined.') : '',
  );

  return $form;
}
