<?php


/**
 * Callback for the add form.
 *
 * @param $js
 * @param $type
 * @param $nid
 * @return the rendered form.
 */
function resources_add($js, $type, $nid) {

  // Setup ctools modal variables.
  $form_state['ajax'] = $js;
  $form_state['build_info']['args'] = array();
  $form_state['cache'] = TRUE;
  $form_state['resource_type'] = $type;

  // Add file entity form.
  form_load_include($form_state, 'pages.inc', 'file_entity');

  // Add more modal information.
  ctools_include('modal');

  // Get the ctools version of the form.
  $output = ctools_modal_form_wrapper('resources_add_form', $form_state);

  // If we are not using javascript then just return the output.
  if (!$js) {
    return $output;
  }

  // Set the ctools form settings to a commands variable we'll use to combine
  // other commands.
  $commands = $output;
  // If a button executes the form but also flags the form to be rebuilt, do not
  // treat it as an actual form submission, and do not close the modal.
  if (!empty($form_state['executed']) && !$form_state['rebuild']) {
    // Add ajax javascript from ctools.
    ctools_include('ajax');

    // Add our javascript.
    ctools_add_js('resources-modal-helper', 'resources');

    $commands = array();
    // If there are status messages, print them.
    if ($messages = theme('status_messages')) {
      $commands[] = ajax_command_html('#console', $messages);
    }

    // If a file ID is present, update the resources.
    if (isset($form_state['file']->fid)) {

      // If we are editing a node then save directly to the usage table.
      if ($nid !== 'new') {
        file_usage_add($form_state['file'], 'resources', 'node', $nid);
      }

      // Add the javascript function we want to call.
      // Pass the rendered output of a resource and the fid.
      $commands[] = array(
        'command' => 'resources_add_resource',
        'resource' => _resources_generate_resource_preview_html($form_state['file']),
        'fid' => $form_state['file']->fid,
      );
    }

    // Close the modal.
    $commands[] = ctools_modal_command_dismiss();
  }

  // Print our form and exit.
  print ajax_render($commands);
  exit();
}

/**
 * Add form.
 *
 * @param $form
 * @param $form_state
 * @return form array
 */
function resources_add_form($form, &$form_state) {
  // Get the resource type information.
  $resource_types = module_invoke_all('resources_type_info');
  $resource_type = $form_state['resource_type'];
  $resource_type_info = $resource_types[$resource_type];
  $file_type = variable_get('resources_' . $resource_type . '_file_type', '');

  // Values that are specific to this resource type that are needed by
  // file_entity functions.
  $perm_values = array(
    //'scheme' => $resource_type_info['scheme'],
    'type' => $file_type,
  );

  foreach ($perm_values as $key => $value) {
    $form[$key] = array(
      '#type' => 'value',
      '#value' => $value,
    );
  }

  // Textfield for the filename.
  $form['filename'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => !empty($form_state['values']['filename']) ? $form_state['values']['filename'] : '',
    '#required' => TRUE,
    '#maxlength' => 255,
    '#weight' => -10,
  );

  if (!empty($resource_type_info['upload'])) {
    // Options for upload widget.
    $options = $resource_type_info['upload'];

    // Upload widget
    $form['upload'] = array(
      '#type' => 'managed_file',
      '#title' => t('Upload a new file'),
      '#upload_location' => file_entity_upload_destination_uri($options),
      '#upload_validators' => file_entity_get_upload_validators($options),
      '#progress_indicator' => 'bar',
      '#required' => TRUE,
      '#pre_render' => array('file_managed_file_pre_render', 'file_entity_upload_validators_pre_render'),
    );

  }

  // Create a file object with just the file type so we can pull the right form.
  $file = new stdClass();
  $file->type = $file_type;

  // Add fields from image bundle.
  field_attach_form('file', $file, $form, $form_state);

  // Add the buttons.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  $form['#validate'][] = 'resources_form_file_validate_handler';

  return $form;
}


/**
 * Resource file fields validate handler for add/edit form.
 *
 * @param $form
 * @param $form_state
 */
function resources_form_file_validate_handler($form, &$form_state) {
  $resource_type_info = resources_get_info($form_state['resource_type']);

  // Validate resource file fields with it's file validate callback, if any.
  if (!empty($resource_type_info['file validate callback'])) {
    call_user_func($resource_type_info['file validate callback'], $form, $form_state, $resource_type_info);
  }
}

/**
 * Submit handler for add form.
 *
 * @param $form
 * @param $form_state
 */
function resources_add_form_submit($form, &$form_state) {
  $file = new stdClass();

  // Only do something if we have a file.
  if (!empty($form_state['values']['upload'])) {
    $file = file_load($form_state['values']['upload']);

    if (!$file) {
      return;
    }

    // Change the file from temporary to permanent.
    $file->status = FILE_STATUS_PERMANENT;
  }

  // Get the resource type information.
  $resource_types = module_invoke_all('resources_type_info');
  $resource_type = $form_state['resource_type'];
  $resource_type_info = $resource_types[$resource_type];
  $file_type = variable_get('resources_' . $resource_type . '_file_type', '');

  // Give it a type and default to displayed.
  $file->type = $file_type;
  $file->display = TRUE;

  // If a filename is specified set the file filename to that.
  if (!empty($form_state['values']['filename'])) {
    $file->filename = $form_state['values']['filename'];
  }

  // Let the resource type owner alter the file data.
  if (!empty($resource_type_info['file presave callback'])) {
    $file = call_user_func($resource_type_info['file presave callback'], $file, $form, $form_state, $resource_type_info);
  }

  // Save the form fields.
  // Keep in mind that the values for the Field API fields must be in
  // $form_state['values'] and not in ['storage']. This is true as long as
  // the fields are on the last page of the multi step form.
  entity_form_submit_build_entity('file', $file, $form, $form_state);

  // Save the file.
  file_save($file);

  // Add the file to the form_state.
  $form_state['file'] = $file;

  // Tell people they did a good job.
  drupal_set_message(t('@type %name was uploaded.', array('@type' => file_entity_type_get_name($file), '%name' => $file->filename)));
}

/**
 * Callback for the edit form.
 *
 * @param $js
 * @param $file
 * @return form
 */
function resources_edit($js, $file) {

  // Setup ctools modal variables.
  $form_state['ajax'] = $js;
  $form_state['build_info']['args'] = array();
  $form_state['cache'] = TRUE;

  // Get the resource type information.
  $resource_types = module_invoke_all('resources_type_info');

  // Figure out the resource type from the scheme.
  foreach ($resource_types as $resource_type =>$resource_type_info) {
    if (variable_get('resources_' . $resource_type . '_file_type', '') === $file->type) {
      $form_state['resource_type'] = $resource_type;
      break;
    }
  }

  // Keep track of the file object.
  $form_state['file'] = $file;

  // Add file entity form.
  form_load_include($form_state, 'pages.inc', 'file_entity');

  // Add more modal information.
  ctools_include('modal');

  // Get the ctools version of the form.
  $output = ctools_modal_form_wrapper('resources_edit_form', $form_state);

  // If we are not using javascript then just return the output.
  if (!$js) {
    return $output;
  }

  // Set the ctools form settings to a commands variable we'll use to combine
  // other commands.
  $commands = $output;

  // If a button executes the form but also flags the form to be rebuilt, do not
  // treat it as an actual form submission, and do not close the modal.
  if (!empty($form_state['executed']) && !$form_state['rebuild']) {
    // Add ajax javascript from ctools.
    ctools_include('ajax');

    // Add our javascript.
    ctools_add_js('resources-modal-helper', 'resources');
    $commands = array();
    // If there are status messages, print them.
    if ($messages = theme('status_messages')) {
      $commands[] = ajax_command_html('#console', $messages);
    }

    // Add the javascript function we want to call.
    // Pass the rendered output of a resource and the fid.
    $commands[] = array(
      'command' => 'resources_edit_resource',
      'resource' => _resources_generate_resource_preview_html($form_state['file']),
      'fid' => $form_state['file']->fid,
    );

    // Close the modal.
    $commands[] = ctools_modal_command_dismiss();
  }
  print ajax_render($commands);
  exit();
}

/**
 * Edit form.
 *
 * @param $form
 * @param $form_state
 * @return form array
 */
function resources_edit_form($form, &$form_state) {
  // Only need to run if we have a file.
  if ($file = $form_state['file']) {

    // Get the resource type information.
    $resource_types = module_invoke_all('resources_type_info');
    $resource_type = $form_state['resource_type'];
    $resource_type_info = $resource_types[$resource_type];

    // Add file_entity methods.
    module_load_include('inc', 'file_entity', 'file_entity.pages');

    // Set the scheme
    /**
    $form['scheme'] = array(
      '#type' => 'value',
      '#value' => $resource_type_info['scheme'],
    );
     **/

    // Basic file information.
    // These elements are just values so they are not even sent to the client.
    foreach (array('fid', 'type', 'uid', 'timestamp') as $key) {
      $form[$key] = array(
        '#type' => 'value',
        '#value' => isset($file->$key) ? $file->$key : NULL,
      );
    }

    // Textfield for the filename.
    $form['filename'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#default_value' => $file->filename,
      '#required' => TRUE,
      '#maxlength' => 255,
      '#weight' => -10,
    );

    if (!empty($resource_type_info['upload'])) {
      // Options for upload widget.
      $replacement_options = array(
        'file_directory' => 'resources',
        'uri_scheme' => 'public',
        'file_extensions' => 'jpg jpeg gif png',
      );

      // Upload widget
      $form['replace_upload'] = array(
        '#type' => 'file',
        '#title' => t('Replace file'),
        '#description' => t('This file will replace the existing file. This action cannot be undone.'),
        '#upload_validators' => file_entity_get_upload_validators($replacement_options),
        '#pre_render' => array('file_entity_upload_validators_pre_render'),
      );
    }

    // Add a preview of the image.
    $form['preview'] = file_view_file($file, 'preview');

    // Add form fields for the file type.
    field_attach_form('file', $file, $form, $form_state);

    // Add the buttons.
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );

    $form['#validate'][] = 'resources_form_file_validate_handler';

    return $form;
  }
}

/**
 * Form validation handler for resources_edit_form().
 */
function resources_edit_form_validate($form, &$form_state) {

  // Handle the replacement file if uploaded.
  if (isset($form_state['values']['replace_upload'])) {
    // Save the file as a temporary file.
    $file = file_save_upload('replace_upload', $form['replace_upload']['#upload_validators']);
    if (!empty($file)) {
      // Put the temporary file in form_values so we can save it on submit.
      $form_state['values']['replace_upload'] = $file;
    }
    elseif ($file === FALSE) {
      // File uploaded failed.
      form_set_error('replace_upload', t('The replacement file could not be uploaded.'));
    }
  }

  // Run entity form validation.
  entity_form_field_validate('file', $form, $form_state);
}

/**
 * Form submission handler for the 'Save' button for resources_edit_form().
 */
function resources_edit_form_submit($form, &$form_state) {
  // Get the file information.
  $file = $form_state['file'];

  // Check if a replacement file has been uploaded.
  if (!empty($form_state['values']['replace_upload'])) {
    $replacement = $form_state['values']['replace_upload'];
    // Move file from temp to permanent home.
    file_unmanaged_copy($replacement->uri, $file->uri, FILE_EXISTS_REPLACE);
  }

  // If a filename is specified set the file filename to that.
  if (!empty($form_state['values']['filename'])) {
    $file->filename = $form_state['values']['filename'];
  }

  // Get the resource type information.
  $resource_types = module_invoke_all('resources_type_info');
  $resource_type = $form_state['resource_type'];
  $resource_type_info = $resource_types[$resource_type];

  // Let the resource type owner alter the file data.
  if (!empty($resource_type_info['file presave callback'])) {
    $file = call_user_func($resource_type_info['file presave callback'], $file, $form, $form_state, $resource_type_info);
  }

  // Run entity form submit handling and save the file.
  entity_form_submit_build_entity('file', $file, $form, $form_state);

  // Save the file.
  file_save($file);

  // Tell them good job.
  $args = array(
    '@type' => file_entity_type_get_name($file),
    '%title' => entity_label('file', $file),
  );
  watchdog('file', '@type: updated %title.', $args);
  drupal_set_message(t('@type %title has been updated.', $args));

  // Clear the page and block caches.
  cache_clear_all();
}

/**
 * Callback for the delete form.
 *
 * @param $js
 * @param $file
 * @return form
 */
function resources_delete($js, $file) {

  // Setup ctools modal variables.
  $form_state['ajax'] = $js;
  $form_state['build_info']['args'] = array();
  $form_state['cache'] = TRUE;

  // Keep track of the file object.
  $form_state['file'] = $file;

  // Add file entity form functions.
  form_load_include($form_state, 'pages.inc', 'file_entity');

  // Add more modal information.
  ctools_include('modal');

  // Get the ctools version of the form.
  $output = ctools_modal_form_wrapper('resources_delete_form', $form_state);

  // If we are not using javascript then just return the output.
  if (!$js) {
    return $output;
  }

  // Set the ctools form settings to a commands variable we'll use to combine
  // other commands.
  $commands = $output;

  // If a button executes the form but also flags the form to be rebuilt, do not
  // treat it as an actual form submission, and do not close the modal.
  if (!empty($form_state['executed']) && !$form_state['rebuild']) {
    // Add ajax javascript from ctools.
    ctools_include('ajax');

    // Add our javascript.
    ctools_add_js('resources-modal-helper', 'resources');
    $commands = array();
    // If there are status messages, print them.
    if ($messages = theme('status_messages')) {
      $commands[] = ajax_command_html('#console', $messages);
    }

    // Add the javascript function we want to call.
    // Pass the rendered output of a resource and the fid.
    if ($form_state['triggering_element']['#name'] === 'delete') {
      $commands[] = array(
        'command' => 'resources_delete_resource',
        'fid' => $form_state['file']->fid,
      );
    }

    // Close the modal.
    $commands[] = ctools_modal_command_dismiss();
  }
  print ajax_render($commands);
  exit();
}

function resources_delete_form($form, &$form_state) {
  // Get the file from the form state.
  $file = $form_state['file'];

  // Render a preview of the file.
  $file_render_array = file_view_file($file, 'preview');
  $rendered_file = drupal_render($file_render_array);

  // Text for confirmation dialog
  $confirm_text = t('Are you sure you want to delete <strong>@name</strong>? This action can not be undone.', array('@name' => $file->filename));

  // Render confirmation dialog.
  $form['description'] = array('#markup' => '<p>' . $confirm_text . '</p>' . $rendered_file);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#name' => 'delete',
    '#value' => t('Confirm'),
  );
  $form['actions']['cancel'] = array(
    '#markup' => l(t('Cancel'), '#',
      array(
        'attributes' => array(
          'class' => 'ctools-close-modal'
        ),
        'external' => TRUE,
      )
    )
  );

  return $form;
}

/**
 * Delete confirm form submit handler.
 */
function resources_delete_form_submit($form, &$form_state) {

  // Make sure the delete button was hit and not cancel.
  if ($form_state['triggering_element']['#name'] === 'delete') {
    module_load_include('pages.inc', 'file_entity');
    $file = $form_state['file'];
    file_delete_multiple(array($file->fid));

    $args = array(
      '@type' => file_entity_type_get_name($file),
      '%title' => entity_label('file', $file),
    );
    watchdog('file', '@type: deleted %title.', $args);
    drupal_set_message(t('@type %title has been deleted.', $args));
  }
}
