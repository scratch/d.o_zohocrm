<?php

/**
 * @file
 * Defines rules' events, actions and conditions.
 */

/**
 * Implements hook_rules_event_info().
 */
function zohocrm_webhooks_rules_event_info() {
  return array(
    'zohocrm_webhooks_webhook' => array(
      'label' => t('Zoho CRM Webhook has been triggered'),
      'module' => 'Zoho CRM',
      'arguments' => array(
        'webhook_name' => array('type' => 'webhook', 'label' => t('Webhook name')),
        'post_data' => array('type' => 'zohocrm_data', 'label' => t('POST data')),
      ),
    ),
  );
}

/**
 * Implements hook_rules_condition_info().
 */
function zohocrm_webhooks_rules_condition_info() {
  return array(
    'zohocrm_webhooks_rules_condition_webhook_name' => array(
      'label' => t('Zoho CRM Webhook name'),
      'arguments' => array(
        'webhook_name' => array('type' => 'webhook', 'label' => t('Zoho CRM Webhook')),
      ),
      'help' => t('Evaluates to TRUE, if webhook name matches the given string.'),
      'module' => 'Zoho CRM',
    ),
  );
}

function zohocrm_webhooks_rules_condition_webhook_name($webhook_name, $settings) {
  return $webhook_name == $settings['name'];
}

function zohocrm_webhooks_rules_condition_webhook_name_form($settings, &$form, &$form_state) {
  $form['settings']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Webhook Name'),
    '#default_value' => $settings['name'],
    '#required' => TRUE,
    '#description' => t('Help Text.'),
  );
}

function zohocrm_webhooks_rules_action_info() {
  $actions = array();

  $defaults = array (
    'parameter' => array(
	'post_data' => array(
	  'type' => 'zohocrm_data',
	  'label' => t('Zoho CRM data'),
	  'description' => t('Zweb: zoho data.'),
	  'save' => TRUE,
	),
      ),
      'group' => t('Zoho'),
      'base' => '',
  );

  $actions['webhooks_action_save_zoho_data'] = $defaults + array(
    'label' => t('Webhooks: Save Zoho data to Drupal'),
    'base' => '',
    'access callback' => 'zohocrm_webhooks_rules_action_save_zoho_data',
  );

  $actions['webhooks_action_delete_zoho_data'] = $defaults + array (
    'label' => t('Webhooks: Delete Drupal Object'),
    'base' => '',
    'access callback' => 'zohocrm_webhooks_rules_action_delete_drupal_object',
  ); 

  return $actions;
}

/**
 * Implements hook_rules_action_info().
function zohocrm_webhooks_rules_action_info() {
  $info = array();
  $info['zohocrm_webhooks_rules_action_save_zoho_data'] = array(
    'label' => t('Save Zoho data to Drupal'),
    'arguments' => array(
      'post_data' => array(
        'type' => 'zohocrm_data',
        'label' => t('Zoho CRM data'),
      ),
    ),
    'help' => t('Help text.'),
    'module' => 'Zoho CRM',
  );

  $info['zohocrm_webhooks_rules_action_delete_drupal_object'] = array(
    'label' => t('Delete Drupal Object'),
    'arguments' => array(
      'post_data' => array(
        'type' => 'zohocrm_data',
        'label' => t('Zoho CRM data'),
      ),
    ),
    'help' => t('Help text.'),
    'module' => 'Zoho CRM',
  );

  return $info;
}
 */

function zohocrm_webhooks_rules_action_save_zoho_data($zoho_data, $settings) {
  zohocrm_save_zoho_data($zoho_data, $settings);
}

function zohocrm_webhooks_rules_action_save_zoho_data_form($settings, &$form) {
  zohocrm_rules_action_form($settings, $form);
}

function zohocrm_webhooks_rules_action_delete_drupal_object($zoho_data, $settings) {
  zohocrm_log($zoho_data, 'ZOHO WEBHOOK DATA');
  zohocrm_is_receiving_zoho_data(TRUE);
  $mapping = zohocrm_mapping_fields_load($settings['mapping']);

  if (empty($mapping['status'])) {
    return;
  }

  if (!isset($zoho_data['Id'])) {
    $key = zohocrm_zoho_module_prefix($mapping['zoho_module']) .' Id';
    if (isset($zoho_data[$key])) {
      $zoho_data['Id'] = $zoho_data[$key];
    }
    else {
      watchdog('zohocrm', t('Zoho Record Id not found in Zoho CRM data .'), array(), WATCHDOG_ERROR);
      return;
    }
  }

  $endpoint = zohocrm_endpoint_info($mapping['drupal_endpoint']);
  $drupal_id = zohocrm_record_mapping_get_drupal_id($mapping['drupal_endpoint'], $mapping['zoho_module'], $zoho_data['Id']);

  if (!empty($drupal_id)) {
    // Impersonate UID 1 to be sure delete operation won't fail.
    global $user;
    $original_user = $user;
    $old_state = session_save_session();
    session_save_session(FALSE);
    /* -- nk. $user = user_load(array('uid' => 1));
     */
    $user = user_load(1);

    // Delete drupal object
    $endpoint['delete_callback']($drupal_id);

    // Restore original session
    $user = $original_user;
    session_save_session($old_state);

    watchdog('zohocrm', t("Deleted Drupal object. Drupal ID: @id"), array('@id' => $drupal_id), WATCHDOG_INFO);
  }
}

function zohocrm_webhooks_rules_action_delete_drupal_object_form($settings, &$form) {
  zohocrm_rules_action_form($settings, $form);
}

/**
 * Implements hook_rules_data_type_info().
 */
function zohocrm_webhooks_rules_data_type_info() {
  return array(
    'webhook' => array(
      'label' => t('Zoho CRM webhook'),
      'class' => 'rules_data_type',
      'savable' => FALSE,
      'identifiable' => FALSE,
    ),
    'zohocrm_data' => array(
      'label' => t('Zoho CRM data'),
      'class' => 'rules_data_type',
      'savable' => FALSE,
      'identifiable' => FALSE,
    ),
  );
}
