<?php

/**
 * Callback for when the file is saved.
 *
 * @param $file
 * @param $form
 * @param $form_state
 * @return bool|object
 */
function resources_resource_twitter_presave($file, $form, $form_state) {
  $language = $form['field_twitter_url']['#language'];
  $widget_id = $form_state['values']['field_twitter_widget_id'][$language][0]['value'];
  $uri = resources_resource_twitter_parse($form_state['values']['field_twitter_url'][$language][0]['url'], $widget_id);
  // @todo: This might go into a base class.
  if (empty($file->uri)) {
    $new_file = file_uri_to_object($uri, TRUE);
    $new_file->type = $file->type;
    $new_file->display = $file->display;
    $new_file->filename = $file->filename;
  }
  else {
    $new_file = $file;
    $new_file->uri = $uri;
  }

  return $new_file;
}

/**
 * Parse the twitter url.
 *
 * @param $url
 * @param null $widget_id
 * @return bool|string
 */
function resources_resource_twitter_parse($url, $widget_id = NULL) {
  $scheme = 'twitter://';
  $url_parts = parse_url($url);

  if (stristr($url_parts['path'], '/status/')) {
    $path_parts = explode('/', $url_parts['path']);
    $status_location = array_search('status', $path_parts, TRUE);
    $user = $path_parts[$status_location - 1];
    $status_id = $path_parts[$status_location + 1];
    $params = array(
      'u' => $user,
      's' => $status_id,
    );
  }
  elseif (stristr($url_parts['path'], '/timelines/')) {
    $path_parts = explode('/', $url_parts['path']);
    $timelines_location = array_search('timelines', $path_parts, TRUE);
    $user = $path_parts[$timelines_location - 1];
    $timeline_id = $path_parts[$timelines_location + 1];
    $params = array(
      'u' => $user,
      't' => $timeline_id,
    );
  }
  elseif (stristr($url_parts['path'], '/search')) {
    $path_parts = drupal_parse_url($url);
    if (!empty($path_parts['path'])) {
      $params['q'] = $path_parts['path'];
    }
  }
  else {
    $path_parts = explode('/', $url_parts['path']);
    if (!empty($path_parts[1])) {
      $params['u'] = $path_parts[1];
    }
  }

  if (empty($params)) {
    return FALSE;
  }

  $default_widget_id = variable_get('resources_default_resource_twitter_widget_id', '');
  $params['w'] = empty($widget_id) ? $default_widget_id : $widget_id;

  $uri = $scheme;
  foreach ($params as $key => $value) {
    $uri .= $key . '/' . $value . '/';
  }

  $uri = trim($uri, '/');
  return $uri;
}

/**
 * Settings form elements.
 *
 * @return mixed
 */
function resources_resource_twitter_settings_form() {
  // We need a default twitter widget id.
  $form['resources_default_resource_twitter_widget_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Twitter Widget ID'),
    '#required' => FALSE,
    '#default_value' => variable_get('resources_default_resource_twitter_widget_id', ''),
  );

  return $form;
}
