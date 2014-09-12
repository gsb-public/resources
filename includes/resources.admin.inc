<?php

/**
 * Administration form for resource settings.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function resources_admin_config($form, $form_state) {

  // Get all the available file types and build an array.
  $file_types = file_type_load_all(TRUE);
  $available_file_types = array();
  foreach ($file_types as $type => $info) {
    $available_file_types[$type] = $info->label;
  }

  // Get all the resource types and build the settings form for one resource
  // each.
  $resource_types = resources_get_info();
  foreach ($resource_types as $resource_type => $resource_type_info) {
    // Provide a wrapper for the resource.
    $form[$resource_type] = array(
      '#type' => 'fieldset',
      '#title' => $resource_type_info['label'],
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    );

    $resource_type_default = variable_get('resources_' . $resource_type . '_file_type', array());

    // Add the file type selection.
    $form[$resource_type]['resources_' . $resource_type . '_file_type'] = array(
      '#type' => 'select',
      '#title' => t('Associated File Type'),
      '#options' => $available_file_types,
      '#required' => FALSE,
      '#default_value' => $resource_type_default,
    );

    $entity_info = entity_get_info();
    $view_mode_info = $entity_info['file']['view modes'];

    // Get all the view modes for the bundle.
    $view_mode_settings = field_view_mode_settings('file', $resource_type_default);

    $view_modes = array();

    // Loop through each view mode to see if they are enabled.
    foreach ($view_mode_settings as $view_mode => $settings) {
      if ($settings['custom_settings']) {
        $view_modes[$view_mode] = $view_mode_info[$view_mode]['label'];
      }
    }

    // Add the file type selection.
    $form[$resource_type]['resources_' . $resource_type . '_view_modes'] = array(
      '#type' => 'select',
      '#title' => t('Views Modes'),
      '#options' => $view_modes,
      '#required' => FALSE,
      '#multiple' => TRUE,
      '#default_value' => variable_get('resources_' . $resource_type . '_view_modes', array()),
    );

    // Only add settings if a callback has been specified for this type.
    if (!empty($resource_type_info['settings callback'])) {
      // Get the specified form elements.
      $form_info = call_user_func($resource_type_info['settings callback']);

      // If we have elements then add the settings wrapper and the form
      // elements.
      if ($form_info) {
        $form[$resource_type]['settings'] = array(
          '#type' => 'fieldset',
          '#title' => t('Settings'),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
        );

        $form[$resource_type]['settings'] += $form_info;
      }
    }
  }

  // Fieldset and Textfield for setting the 'Browser Preview View Mode'

  $form['resources_browser_preview_view_mode_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => 'Browser Preview',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['resources_browser_preview_view_mode_fieldset']['resources_browser_preview_view_mode'] = array(
    '#type' => 'textfield',
    '#title' => t('View Mode'),
    '#required' => FALSE,
    '#default_value' => variable_get('resources_browser_preview_view_mode', 'preview'),
  );

  return system_settings_form($form);
}
