<?php

function resources_admin_config($form, $form_state) {
  // Associate resource types with a file type.
  $form['file_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Associated file types'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $file_types = file_type_load_all(TRUE);
  $available_file_types = array();
  foreach ($file_types as $type => $info) {
    if ($type !== 'application') {
      $available_file_types[$type] = $info->label;
    }
  }

  $resource_types = module_invoke_all('resources_type_info');
  foreach ($resource_types as $resource_type => $resource_type_info) {
    $form['file_types']['resources_' . $resource_type . '_file_type'] = array(
      '#type' => 'select',
      '#title' => $resource_type_info['label'],
      '#options' => $available_file_types,
      '#required' => FALSE,
      '#default_value' => variable_get('resources_' . $resource_type . '_file_type', array()),
    );
  }

  return system_settings_form($form);
}

function resources_resource_twitter_admin_config($form, $form_state) {
  $form['resources_default_resource_twitter_widget_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Twitter Widget ID'),
    '#required' => FALSE,
    '#default_value' => variable_get('resources_default_resource_twitter_widget_id', ''),
  );

  return system_settings_form($form);
}