<?php

/**
 * @file
 */

function zohocrm_export_form($form, &$form_state) {
  $form['mapping'] = array(
    '#type' => 'select',
    '#title' => t('Mapping'),
    '#description' => t('Name of the mapping which will be used for exporting data.'),
    '#options' => zohocrm_mappings_select_options(),
    '#required' => TRUE,
  );

  $form['max_records'] = array(
    '#type' => 'select',
    '#title' => t('Number of records per CSV file'),
    '#description' => t('Maximum number of records per CSV file.
                        See <a href="@url">Zoho CRM documentation</a> for more details.
                        If exported data exceeds this number it will be split into multiple
                        files.', array('@url' => url('http://www.zoho.com/crm/help/data-administration/import-data.html#Import_Limits'))),
    '#options' => array(
      '1500' => '1,500',
      '5000' => '5,000',
      '10000' => '10,000',
      '20000' => '20,000',
    ),
  );

  $form['include_mapped'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include mapped records.'),
    '#description' => t('Include drupal records that are already mapped to Zoho CRM records.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Export'),
  );

  return $form;
}

function zohocrm_export_form_validate($form, &$form_state) {
  global $user;

  /*
  $path = file_create_path(variable_get('zohocrm_export_directory', 'zohocrm_export') .'/'. $user->uid);
  $result = file_check_directory($path, FILE_CREATE_DIRECTORY);
  if ($result === FALSE) {
   */
  $path = variable_get('zohocrm_export_directory', 'zohocrm_export' . '/' . $user->uid);
  $result = file_prepare_directory($path, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
  if ($result === FALSE) {
    form_set_error('file_system', t('Files directory is not writable.
                    Check your <a href="!file_system">file system settings</a>
                    and try again.', array('!file_system' => url('admin/settings/file-system'))));
  }
  $form_state['values']['save_path'] = $path;

  if (empty($form_state['values']['max_records'])) {
    $form_state['values']['max_records'] = ZOHOCRM_CSV_RECORDS_LIMIT;
  }
}

function zohocrm_export_form_submit($form, &$form_state) {

  $mapping = zohocrm_mapping_fields_load($form_state['values']['mapping']);
  $endpoint = zohocrm_endpoint_info($mapping['drupal_endpoint']);

  if (!empty($form_state['values']['include_mapped'])) {
    $endpoint['query_object_selected'] = $endpoint['query_object']['all'];
  }
  else {
    $endpoint['query_object_selected'] = $endpoint['query_object']['notmapped'];
  }

  $batch = array(
    'title' => t('Exporting Drupal data.'),
    'progress_message' => '',
    'operations' => array(array('zohocrm_export_batch_operation', array($mapping, $endpoint, $form_state['values']['save_path'], $form_state['values']['max_records']))),
    'finished' => 'zohocrm_export_batch_finished',
  );

  batch_set($batch);
  $form_state['redirect'] = 'admin/settings/zohocrm/export/downloads';
}

function zohocrm_export_downloads() {
  global $user;

  $export_dir = variable_get('zohocrm_export_directory', 'zohocrm_export');
  $sql = "SELECT * FROM {files} WHERE uid = %d AND filepath LIKE '%%/". $export_dir ."/%s/%%' ORDER BY timestamp DESC, filename";
  $result = db_query($sql, $user->uid, $user->uid);

  $downloads = array();
  while ($file = db_fetch_object($result)) {
    if (!file_exists($file->filepath)) {
      continue;
    }
    $downloads[$file->timestamp][] = $file;
  }

  if (empty($downloads)) {
    return t('There are no exported files available for download.');
  }

  if (count($downloads) > 1) {
    $previous = array_slice($downloads, 1, NULL, TRUE);
    $downloads = array_slice($downloads, 0, 1, TRUE);
  }

  $rows = _zohocrm_export_table_rows($downloads);
  $output = theme('table', array(), $rows, array('id' => 'latest-export'), t('Most Recent Export'));

  if (!empty($previous)) {
    $rows = _zohocrm_export_table_rows($previous);
    $output .= theme('table', array(), $rows, array('id' => 'previous-exports'), t('Previous Exports'));
  }

  return $output;
}

function _zohocrm_export_table_rows($downloads) {
  $rows = array();
  foreach ($downloads as $timestamp => $files) {
    list(, $module) = explode('_', $files[0]->filename);
    $rows[][] = array('data' => t('Files for Zoho CRM module %module exported on @date', array('%module' => $module, '@date' => format_date($timestamp))), 'class' => 'section-label', 'colspan' => 3, 'style' => 'font-weight: bold');

    foreach ($files as $file) {
      $row = array (
        check_plain ($file->filename),
        format_size($file->filesize),
        /* -- nk. file_create_path without any param implies the ./files dir.
         l(t('Download'), 'system/files/' . str_replace(file_create_path() .'/', '', $file->filepath)),

	 REVISIT: path changed from ./file to 'zohocrm_export_directory' varialbe 
        */
        l(t('Download'), 'system/files/' . str_replace(variable_get('zohocrm_export_directory', 'zohocrm_export') . '/', '', $file->filepath)),
      );
      $rows[] = $row;
    }
  }

  return $rows;
}
