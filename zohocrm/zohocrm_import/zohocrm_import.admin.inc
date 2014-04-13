<?php

/**
 * @file
 */

function zohocrm_import_form(&$form_state) {

  if (isset($form_state['storage']['file'])) {
    // Display second form page.
    return zohocrm_import_form_review_page($form_state);
  }

  $form = array(
    '#attributes' => array('enctype' => "multipart/form-data"),
  );

  $form['csv_upload'] = array(
    '#type' => 'file',
    '#title' => t('Upload CSV file'),
    '#description' => t('CSV file containing Zoho CRM data.'),
  );

  $form['mapping'] = array(
    '#type' => 'select',
    '#title' => t('Mapping'),
    '#description' => t('Name of the mapping which will be used to map Zoho fields to Drupal.'),
    '#options' => zohocrm_mappings_select_options(),
    '#required' => TRUE,
  );

  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );

  return $form;
}

/**
 * Second form page.
 */
function zohocrm_import_form_review_page(&$form_state) {
  $form = array();

  $form['description'] = array(
    '#type' => 'markup',
    '#value' => t('@count records will be imported from uploaded CSV file.
                  Before proceeding, review the sample record below and confirm
                  that values got parsed correctly.', array('@count' => $form_state['storage']['count'])),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );

  $items = array();
  foreach ($form_state['storage']['preview_data'] as $label => $value) {
    $items[] = '<strong>'. $label .':</strong> '. $value;
  }
  $form['preview'] = array(
    '#type' => 'markup',
    '#value' => theme('item_list', $items),
  );

  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#validate' => array('zohocrm_import_cancel_validate'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );

  return $form;
}

function zohocrm_import_form_validate($form, &$form_state) {
  if (isset($form_state['storage']['file'])) {
    // We are at the review page. Nothing to validate here.
    return;
  }

  if (empty($_FILES['files']['name']['csv_upload'])) {
    form_set_error('csv_upload', t('@field field is required', array('@field' => $form['csv_upload']['#title'])));
  }

  $validators = array(
    'file_validate_extensions' => array('csv'),
  );
  if ($file = file_save_upload('csv_upload', $validators)) {
    $form_state['storage']['file'] = $file;
  }
  else {
    form_set_error('', t('File upload failed.'));
  }
}

function zohocrm_import_cancel_validate($form, &$form_state) {
  unset($form_state['values']);
  unset($form_state['storage']);

  $form_state['rebuild'] = TRUE;
}

function zohocrm_import_form_submit($form, &$form_state) {
  // Second page got submitted.
  if (isset($form_state['storage']['batch'])) {
    batch_set($form_state['storage']['batch']);
    unset($form_state['storage']);
    $form_state['redirect'] = 'admin/settings/zohocrm';
    return;
  }

  // First page got submitted.
  $file = $form_state['storage']['file'];

  file_set_status($file, FILE_STATUS_PERMANENT);

  // Read the uploaded file and prepare data for import.
  $f = fopen($file->filepath, "r");
  if ($f === FALSE) {
    drupal_set_message(t('Uploaded file could not be opened. Import failed.'), 'error');
    return;
  }

  // Prepare preview data.
  $header = fgetcsv($f);
  // Remember position of the first row with data so that import can be started
  // directly from that position (skipping CSV header).
  $offset = ftell($f);
  $row = fgetcsv($f);
  $record = zohocrm_import_prepare_record($row, $header, $form_state['values']['mapping']);

  // Count total number of rows.
  $count = 1;
  while (($row = fgetcsv($f)) !== FALSE) {
    $count++;
  }

  fclose($f);

  $settings = array('mapping' => $form_state['values']['mapping']);
  $data = array(
    'count' => $count,
    'offset' => $offset,
    'header' => $header,
  );
  $batch = array(
    'title' => t('Importing Zoho CRM CSV dump'),
    'progress_message' => '',
    'operations' => array(array('zohocrm_import_batch_operation', array($file, $data, $settings))),
    'finished' => 'zohocrm_import_batch_finished'
  );

  $form_state['storage']['count'] = $count;
  $form_state['storage']['preview_data'] = $record;
  $form_state['storage']['batch'] = $batch;
}
