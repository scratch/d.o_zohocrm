<?php

/**
 * @file
 * Admin interface.
 */

 /**
  * Module configuration form.
  */
function zohocrm_admin_settings($form, &$form_state) {
  $form['zohocrm_client_disabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable sending of Drupal data to Zoho CRM'),
    '#description' => t('This will completely disable sending of any Drupal data to Zoho CRM even when field mappings and rules are configured and enabled. For exaple you may want to use this feature in your dev and testing environment when you want to avoid sending test data to Zoho CRM.'),
    '#default_value' => variable_get('zohocrm_client_disabled', 0),
    '#weight' => -30,
  );

  $form['zohocrm_authtoken'] = array(
    '#type' => 'textfield',
    '#title' => t('Zoho CRM API Authentication Token'),
    '#description' => t('See <a href="@url">Zoho CRM API documentation</a>', array('@url' => url('http://www.zoho.com/crm/help/api/using-authentication-token.html'))),
    '#default_value' => variable_get('zohocrm_authtoken', ''),
    '#required' => TRUE,
    '#size' => 30,
    '#weight' => -12,
  );

  $zones = timezone_identifiers_list();
  $options = array('' => '');
  foreach ($zones as $zone) {
    $options[$zone] = $zone;
  }

  $form['zohocrm_timezone'] = array(
    '#type' => 'select',
    '#title' => t('Zoho CRM Timezone'),
    '#description' => t('Timezone of Zoho CRM user whose Auth Token is configured above. Required
                        for correct synchronization of dates and times between Drupal and Zoho CRM.
                        In order to find the timezone log in to your Zoho CRM account and go to:
                        Setup -> Users & Permissions -> User Name -> Locale Information -> Timezone'),
    '#default_value' => variable_get('zohocrm_timezone', ''),
    '#options' => $options,
    '#required' => TRUE,
    '#weight' => -8,
  );

  $options = array();
  $formats = filter_formats();
  foreach ($formats as $format) {
    $options[$format->format] = $format->name;
  }

  $form['zohocrm_filter_format'] = array(
    '#type' => 'select',
    '#title' => t('Input Format'),
    '#description' => t("Input format to be used when saving Zoho data to Drupal's textarea fields."),
    '#default_value' => variable_get('zohocrm_filter_format', filter_fallback_format()),
    '#options' => $options,
    '#weight' => -4,
  );

  return system_settings_form($form);
}

/**
 * Lists available mappings.
 */
function zohocrm_mapping_list() {
  $authtoken = variable_get('zohocrm_authtoken', '');
  if (empty($authtoken)) {
    drupal_set_message(t('Please configure <a href="@url">Zoho CRM API Authentication Token</a>.', array('@url' => url('admin/settings/zohocrm/settings'))), 'error');
    return '';
  }

  $output = l(t('Add new mapping'), 'admin/settings/zohocrm/mapping/add');

  $endpoints = zohocrm_endpoint_info();

  $rows = array();
  $mappings = zohocrm_get_mappings(FALSE);
  foreach ($mappings as $mapping) {
    if ($mapping['status']) {
      $class = '';
      $toggle_status = l(t('Disable'), 'admin/settings/zohocrm/mapping/' . $mapping['mapping_name'] . '/disable');
    }
    else {
      $class = 'disabled';
      $toggle_status = l(t('Enable'), 'admin/settings/zohocrm/mapping/' . $mapping['mapping_name'] . '/enable');
    }
    $links = array(
      l(t('Edit Description'), 'admin/settings/zohocrm/mapping/' . $mapping['mapping_name'] . '/edit'),
      l(t('Fields'), 'admin/settings/zohocrm/mapping/' . $mapping['mapping_name'] . '/fields'),
      $toggle_status,
      l(t('Delete'), 'admin/settings/zohocrm/mapping/' . $mapping['mapping_name'] . '/delete'),
    );

    $rows[] = array(
      'data' => array(
        check_plain($mapping['mapping_name']),
        check_plain($mapping['description']),
        check_plain($endpoints[$mapping['drupal_endpoint']]['label']),
        check_plain($mapping['zoho_module']),
        implode(' | ', $links),
      ),
      'class' => array($class),
    );
  }

  $header = array(
    t('Name'),
    t('Description'),
    t('Drupal Endpoint'),
    t('Zoho Module'),
    '',
  );
  $output .= theme('table', array('header' => $header, 'rows' => $rows));

  return $output;
}

/**
 * Form for editing Mapping description.
 */
function zohocrm_mapping_edit_form($form, &$form_state, $mapping) {
  $form['mapping_name_ro'] = array(
    '#type' => 'textfield',
    '#title' => t('Mapping Name'),
    '#description' => t('Only alphanumerical characters and underscores.'),
    '#value' => check_plain($mapping['mapping_name']),
    '#disabled' => TRUE,
    '#size' => 20,
  );

  $form['mapping_name'] = array(
    '#type' => 'value',
    '#value' => $mapping['mapping_name'],
  );

  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Mapping Description'),
    '#description' => t('Short description of the mapping.'),
    '#default_value' => check_plain($mapping['description']),
    '#maxlength' => 255,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit handler for zohocrm_mapping_edit_form().
 */
function zohocrm_mapping_edit_form_submit($form, &$form_state) {
  db_update('zohocrm_mapping')
    ->fields(array('description' => $form_state['values']['description']))
    ->condition('mapping_name', $form_state['values']['mapping_name'])
    ->execute();

  $form_state['redirect'] = 'admin/settings/zohocrm/mapping';
}

/**
 * Confirm form for deleting a mapping.
 */
function zohocrm_mapping_delete_form($form, &$form_state, $mapping) {
  $form['mapping_name'] = array(
    '#type' => 'value',
    '#value' => $mapping['mapping_name'],
  );

  return confirm_form($form,
    t('Are you sure you want to delete mapping %title?', array('%title' => $mapping['mapping_name'])),
    'admin/settings/zohocrm/mapping',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler for zohocrm_mapping_delete_form().
 */
function zohocrm_mapping_delete_form_submit($form, &$form_state) {
  zohocrm_mapping_delete($form_state['values']['mapping_name']);

  $form_state['redirect'] = 'admin/settings/zohocrm/mapping';
}

/**
 * Confirm form for disabling a mapping.
 */
function zohocrm_mapping_disable_form($form, &$form_state, $mapping) {
  $form['mapping_name'] = array(
    '#type' => 'value',
    '#value' => $mapping['mapping_name'],
  );

  return confirm_form($form,
    t('Are you sure you want to disable mapping %title?', array('%title' => $mapping['mapping_name'])),
    'admin/settings/zohocrm/mapping',
    t('IMPORTANT: This will also disable execution of all rules actions that are configured to use this mapping.'),
    t('Disable'),
    t('Cancel')
  );
}

/**
 * Submit handler for zohocrm_mapping_disable_form().
 */
function zohocrm_mapping_disable_form_submit($form, &$form_state) {
  zohocrm_mapping_disable($form_state['values']['mapping_name']);

  $form_state['redirect'] = 'admin/settings/zohocrm/mapping';
}

/**
 * Confirm form for enabling a mapping.
 */
function zohocrm_mapping_enable_form($form, &$form_state, $mapping) {
  $form['mapping_name'] = array(
    '#type' => 'value',
    '#value' => $mapping['mapping_name'],
  );

  return confirm_form($form,
    t('Are you sure you want to enable mapping %title?', array('%title' => $mapping['mapping_name'])),
    'admin/settings/zohocrm/mapping',
    t('IMPORTANT: This will also enable execution of all rules actions that are configured to use this mapping.'),
    t('Enable'),
    t('Cancel')
  );
}

/**
 * Submit handler for zohocrm_mapping_enable_form().
 */
function zohocrm_mapping_enable_form_submit($form, &$form_state) {
  zohocrm_mapping_enable($form_state['values']['mapping_name']);

  $form_state['redirect'] = 'admin/settings/zohocrm/mapping';
}

/**
 * Defines a form for configuring field mappings.
 */
function zohocrm_mapping_fields_form($form, &$form_state, $mapping) {
  $form['#tree'] = TRUE;

  $endpoints = zohocrm_endpoint_info();

  $args = array(
    '@label' => $endpoints[$mapping['drupal_endpoint']]['label'],
    '@module' => $mapping['zoho_module'],
  );
  $title = t('@label to Zoho @module', $args);
  $description = check_plain($mapping['description']);
  $form['description'] = array(
    '#type' => 'markup',
    '#value' => '<h2>' . $title . '</h2><p>' . $description . '</p>',
  );

  $defaults = isset($mapping['fields']) ? $mapping['fields'] : array();
  $default_option = array('' => '<' . t('none') . '>');

  // Drupal to Zoho.
  $form['dtz'] = array(
    '#type' => 'fieldset',
    '#title' => t('Drupal to Zoho'),
    '#description' => t('This mapping will be used for sending Drupal data to Zoho.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#theme' => 'zohocrm_mapping_fields_table_dtz',
  );

  $drupal_options = zohocrm_drupal_select_options($mapping['drupal_endpoint']);
  $zoho_fields = zohocrm_get_zoho_fields_info($mapping['zoho_module']);

  $i = 0;
  foreach ($zoho_fields as $section_name => $section_fields) {
    $form['dtz'][$i++]['section'] = array(
      '#markup' => check_plain($section_name),
    );

    foreach ($section_fields as $field_config) {
      if ($field_config['isreadonly']) {
        continue;
      }

      $label = $field_config['label'];

      $form['dtz'][$i]['field_name'] = array(
        '#markup' => check_plain($field_config['dv']),
        '#required' => $field_config['req'],
      );

      $size = NULL;
      $maxlength = NULL;
      if ($field_config['type'] == 'Pick List') {
        $type = 'select';
      }
      elseif ($field_config['type'] == 'TextArea') {
        $type = 'textarea';
        $maxlength = $field_config['maxlength'];
      }
      else {
        $type = 'textfield';
        $size = 20;
        $maxlength = $field_config['maxlength'];
      }
      $form['dtz'][$i]['default_value'] = array(
        '#type' => $type,
        '#default_value' => isset($defaults[$label]['default_value']) ? check_plain($defaults[$label]['default_value']) : '',
        '#maxlength' => $maxlength,
        '#size' => $size,
      );
      if ($field_config['type'] == 'Pick List') {
        array_shift($field_config['values']);
        $options = $default_option + array_combine($field_config['values'], $field_config['values']);
        $form['dtz'][$i]['default_value']['#options'] = $options;
      }

      $form['dtz'][$i]['src'] = array(
        '#type' => 'select',
        '#default_value' => isset($defaults[$label]['src']) ? check_plain($defaults[$label]['src']) : '',
        '#options' => $drupal_options,
      );

      $form['dtz'][$i]['sync_direction'] = array(
        '#type' => 'value',
        '#value' => ZOHOCRM_SYNC_DRUPAL_TO_ZOHO,
      );

      $form['dtz'][$i]['dst'] = array(
        '#type' => 'value',
        '#value' => $label,
      );

      $form['dtz'][$i]['mfid'] = array(
        '#type' => 'value',
        '#value' => isset($defaults[$label]['mfid']) ? check_plain($defaults[$label]['mfid']) : '',
      );

      $i++;
    }
  }

  if (module_exists('zohocrm_webhooks') || module_exists('zohocrm_import')) {
    // Zoho to Drupal.
    $form['ztd'] = array(
      '#type' => 'fieldset',
      '#title' => t('Zoho to Drupal'),
      '#description' => t('This mapping will be used for saving Zoho data to Drupal.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#theme' => 'zohocrm_mapping_fields_table_ztd',
    );

    $zoho_options = zohocrm_zoho_select_options($mapping['zoho_module']);
    $drupal_fields = zohocrm_drupal_fields($mapping['drupal_endpoint']);

    $i = 0;
    foreach ($drupal_fields as $section_name => $section_fields) {
      $form['ztd'][$i++]['section'] = array(
        '#markup' => check_plain($section_name),
      );

      foreach ($section_fields as $key => $field_config) {
        $label = $field_config['label'];

        $form['ztd'][$i]['field_name'] = array(
          '#markup' => check_plain($label),
        );

        $form['ztd'][$i]['src'] = array(
          '#type' => 'select',
          '#default_value' => isset($defaults[$key]['src']) ? $defaults[$key]['src'] : '',
          '#options' => $zoho_options,
        );

        $form['ztd'][$i]['default_value'] = array(
          '#type' => 'value',
          '#value' => '',
        );

        $form['ztd'][$i]['sync_direction'] = array(
          '#type' => 'value',
          '#value' => ZOHOCRM_SYNC_ZOHO_TO_DRUPAL,
        );

        $form['ztd'][$i]['dst'] = array(
          '#type' => 'value',
          '#value' => $key,
        );

        $form['ztd'][$i]['mfid'] = array(
          '#type' => 'value',
          '#value' => isset($defaults[$key]['mfid']) ? $defaults[$key]['mfid'] : '',
        );

        $i++;
      }
    }

    if (module_exists('zohocrm_webhooks')) {
      $form['webhooks_help'] = array(
        '#type' => 'fieldset',
        '#title' => t('Webhook XML data parameter'),
        '#description' => t('On Zoho CRM Webhook configuration page go to section "Parameters in the
                            User Defined Format", create parameter named "xmldata" and paste this
                            XML code in the value field. This code can be used as it is or customized
                            to fit your own specific requirements.'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

      $xmldata = zohocrm_webhooks_xmldata($mapping['zoho_module']);
      $form['webhooks_help']['xmldata'] = array(
        '#type' => 'markup',
        '#value' => check_plain($xmldata),
        '#prefix' => '<pre>',
        '#suffix' => '</pre>',
      );
    }
  }

  $form['mapping_name'] = array(
    '#type' => 'value',
    '#value' => $mapping['mapping_name'],
  );

  $form['drupal_endpoint'] = array(
    '#type' => 'value',
    '#value' => $mapping['drupal_endpoint'],
  );

  $form['zoho_module'] = array(
    '#type' => 'value',
    '#value' => $mapping['zoho_module'],
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit handler for zohocrm_mapping_fields_form().
 */
function zohocrm_mapping_fields_form_submit($form, &$form_state) {
  foreach ($form_state['values']['dtz'] as $record) {
    zohocrm_mapping_field_save($form_state['values']['mapping_name'], $record);
  }
  if (isset($form_state['values']['ztd'])) {
    foreach ($form_state['values']['ztd'] as $record) {
      zohocrm_mapping_field_save($form_state['values']['mapping_name'], $record);
    }
  }

  $form_state['redirect'] = 'admin/settings/zohocrm/mapping';
}

/**
 * Theme function for "Drupal to Zoho" fieldset.
 *
 * Renders fields as a HTML table.
 *
 * @see zohocrm_mapping_fields_form()
 */
function theme_zohocrm_mapping_fields_table_dtz($variables) {
  $vars = array(
    'header' => array(t('Zoho Field'), t('Drupal Field'), t('Default Value')),
    'rows' => zohocrm_mapping_fields_rows($variables['element']),
    'attributes' => array(
      'id' => 'mapping-fields-dtz',
    ),
  );

  return theme('table', $vars);
}

/**
 * Theme function for "Zoho to Drupal" fieldset.
 *
 * Renders fields as a HTML table.
 *
 * @see zohocrm_mapping_fields_form()
 */
function theme_zohocrm_mapping_fields_table_ztd($variables) {
  $vars = array(
    'header' => array(t('Drupal Field'), t('Zoho Field'), t('Default Value')),
    'rows' => zohocrm_mapping_fields_rows($variables['element']),
    'attributes' => array(
      'id' => 'mapping-fields-ztd',
    ),
  );

  return theme('table', $vars);
}

/**
 * Helper function for generating table rows.
 *
 * @see theme_zohocrm_mapping_fields_table_dtz()
 * @see theme_zohocrm_mapping_fields_table_ztd()
 */
function zohocrm_mapping_fields_rows($form) {
  $rows = array();
  foreach (element_children($form) as $key) {
    $field = &$form[$key];
    $row = array();
    if (isset($field['section'])) {
      $row[] = array(
        'data' => drupal_render($field['section']),
        'class' => 'section-label',
        'colspan' => 3,
        'style' => 'font-weight: bold',
      );
    }
    else {
      if (isset($field['field_name']['#required']) && $field['field_name']['#required']) {
        $required_class = ' required';
        $required = '<span class="form-required" title="' . t('This field is required.') . '"> *</span>';
      }
      else {
        $required = $required_class = '';
      }
      $row[] = array('data' => drupal_render($field['field_name']) . $required, 'class' => 'field-name' . $required_class);
      $row[] = array('data' => drupal_render($field['src']), 'class' => 'source-field');
      $row[] = array('data' => drupal_render($field['default_value']) . drupal_render($field['dst']) . drupal_render($field['mfid']) . drupal_render($field['sync_direction']), 'class' => 'default-value');
    }
    $rows[] = $row;
  }

  return $rows;
}

/**
 * Form for creating new Mapping.
 */
function zohocrm_mapping_add_form($form, &$form_state) {
  $endpoints = zohocrm_endpoint_info();

  $options = array();
  foreach ($endpoints as $endpoint) {
    $options[$endpoint['type']] = $endpoint['label'];
  }
  $options = array('' => '') + $options;

  $form['mapping_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Mapping Name'),
    '#description' => t('Only alphanumerical characters and underscores.'),
    '#required' => TRUE,
    '#size' => 20,
  );

  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Mapping Description'),
    '#description' => t('Short description of the mapping.'),
    '#maxlength' => 255,
  );

  $form['drupal_endpoint'] = array(
    '#type' => 'select',
    '#title' => t('Drupal Endpoint'),
    '#options' => $options,
    '#required' => TRUE,
  );

  $form['zoho_module'] = array(
    '#type' => 'textfield',
    '#title' => t('Zoho Module'),
    '#description' => t('Name of the Zoho module (Contacts, Accounts, etc). For
                        custom modules always use internal module name starting
                        with "CustomModule". This name can be found on Modules
                        config page in Zoho CRM. For example: CustomModule1.'),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );

  return $form;
}

/**
 * Validation handler for zohocrm_mapping_add_form().
 */
function zohocrm_mapping_add_form_validate($form, &$form_state) {
  if (preg_match('/[^a-zA-Z0-9_]/', $form_state['values']['mapping_name'])) {
    form_set_error('mapping_name', t('Mapping Name can contain only alpha-numeric characters and underscores.'));
  }

  $mappings = array_keys(zohocrm_get_mappings(FALSE));
  if (in_array($form_state['values']['mapping_name'], $mappings)) {
    form_set_error('mapping_name', t('This Mapping Name already exists. Please chose another one.'));
  }

  $module = $form_state['values']['zoho_module'];
  $builtin = in_array($module, zohocrm_get_zoho_modules()) ? TRUE : FALSE;
  $custom = preg_match('/CustomModule[0-9]+/', $module);
  if (!$builtin && !$custom) {
    form_set_error('zoho_module', t('Ivalid Zoho module name.'));
  }
}

/**
 * Submit handler for zohocrm_mapping_add_form().
 */
function zohocrm_mapping_add_form_submit($form, &$form_state) {
  try {
    db_insert('zohocrm_mapping')
      ->fields(array(
        'mapping_name'    => $form_state['values']['mapping_name'],
        'description'     => $form_state['values']['description'],
        'drupal_endpoint' => $form_state['values']['drupal_endpoint'],
        'zoho_module'     => $form_state['values']['zoho_module'],
        'status'          => 1,
      ))
      ->execute();
  }
  catch (Exception $e) {
    drupal_set_message(t('Adding new mapping failed.'), 'error');
    watchdog('zohocrm', 'Adding new mapping failed with exception: @message', array('@message' => $e->getMessage()), WATCHDOG_ERROR);
  }

  $form_state['redirect'] = 'admin/settings/zohocrm/mapping';
}

/**
 * Provides options for select form element.
 *
 * Options represent a list of Drupal fields that can be used as a source for
 * "Drupal to Zoho" sync.
 *
 * @param string $drupal_endpoint
 *   Name of the Drupal endpoint.
 *
 * @return array
 *   Options in the format accepted by the '#options' property of the Form API.
 */
function zohocrm_drupal_select_options($drupal_endpoint) {
  $options = array(
    'Global' => array(
      '' => t('Do not sync'),
      '__use_default__' => t('Use default value'),
    ),
  );

  $fields_info = zohocrm_endpoint_fields_info($drupal_endpoint);

  foreach ($fields_info as $key => $field) {
    if ($field['sync_direction'] & ZOHOCRM_SYNC_DRUPAL_TO_ZOHO) {
      $options[$field['group']][$key] = $field['label'];
    }
  }

  return $options;
}

/**
 * Provides options for select form element.
 *
 * Options represent a list of Zoho CRM fields that can be used as a source for
 * "Zoho to Drupal" sync.
 *
 * @param string $zoho_module
 *   Name of the Zoho CRM module.
 *
 * @return array
 *   Options in the format accepted by the '#options' property of the Form API.
 */
function zohocrm_zoho_select_options($zoho_module) {
  $zoho_options = array(
    'Global' => array(
      '' => t('Do not sync'),
      // '__use_default__' => t('Use default value'),
      ZOHOCRM_ZOHO_RECORD_URL => ZOHOCRM_ZOHO_RECORD_URL,
    ),
  );

  $zoho_fields = zohocrm_get_zoho_fields_info($zoho_module);

  foreach ($zoho_fields as $section_name => $section_fields) {
    foreach ($section_fields as $field) {
      $zoho_options[$section_name][$field['label']] = $field['dv'];
    }
  }

  $sections = array_keys($zoho_options);
  if (count($sections) > 1) {
    $second_optgroup = $sections[1];

    if ($zoho_module == 'Contacts' || $zoho_module == 'Leads') {
      // Add "Full Name" fields to Contacts and Leads.
      $zoho_options[$second_optgroup] = array(ZOHOCRM_FULLNAME_LASTFIRST => ZOHOCRM_FULLNAME_LASTFIRST) + $zoho_options[$second_optgroup];
      $zoho_options[$second_optgroup] = array(ZOHOCRM_FULLNAME_FIRSTLAST => ZOHOCRM_FULLNAME_FIRSTLAST) + $zoho_options[$second_optgroup];
    }

    // Add record Id as first field in the second optgroup.
    $key = zohocrm_zoho_module_prefix($zoho_module) . ' Id';
    $label = t('Zoho Record Id');
    $zoho_options[$second_optgroup] = array($key => $label) + $zoho_options[$second_optgroup];
  }

  // Add fields from related modules.
  $related = zohocrm_get_zoho_fields_related($zoho_module);
  foreach ($related as $related_module => $data) {
    foreach ($data['fields'] as $field) {
      $zoho_options[$data['section']][$field] = $field;

      // Remove duplicate fields.
      if (!empty($second_optgroup) && isset($zoho_options[$second_optgroup][$field])) {
        unset($zoho_options[$second_optgroup][$field]);
      }
    }
  }

  return $zoho_options;
}

/**
 * Lists Drupal fields that can be used as destination for Zoho to Drupal sync.
 *
 * @param string $endpoint
 *   Name of the Drupal endpoint.
 *
 * @return array
 *   Array of Drupal fields.
 */
function zohocrm_drupal_fields($endpoint) {
  $fields = array();
  $endpoint_fields = zohocrm_endpoint_fields_info($endpoint);

  foreach ($endpoint_fields as $key => $field) {
    if ($field['sync_direction'] & ZOHOCRM_SYNC_ZOHO_TO_DRUPAL) {
      $fields[$field['group']][$key] = array(
        'label' => $field['label'],
      );
    }
  }

  return $fields;
}
