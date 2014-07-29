<?php

// Define patters of supported twitter URLs.
const TWITTER_STATUS_URL_PATTERN = '@twitter\.com/([^\/]+)/status/([0-9]*)$@i';
const TWITTER_COLLECTION_URL_PATTERN = '@twitter\.com/([^\/]+)/timelines/([0-9]*)$@i';
const TWITTER_USER_TIMELINE_URL_PATTERN = '@twitter\.com/([^/\?#]*)$@i';
const TWITTER_WIDGET_ID_URL_PATTERN = '@twitter.com/settings/widgets/([0-9]+)/edit$@i';

/**
 * Validate handler.
 *  - Validate twitter URL.
 */
function resources_resource_twitter_validate($form, &$form_state) {
  $language = $form['field_twitter_url']['#language'];

  $url = $form_state['values']['field_twitter_url'][$language][0]['url'];

  // Validate patters for various twitter URLs we support.
  $patterns = array(
    TWITTER_STATUS_URL_PATTERN,
    TWITTER_COLLECTION_URL_PATTERN,
    TWITTER_USER_TIMELINE_URL_PATTERN,
    TWITTER_WIDGET_ID_URL_PATTERN
  );

  $valid_url = FALSE;
  foreach ($patterns as $pattern) {
    if (preg_match($pattern, $url)) {
      // We need at least one pattern to match.
      $valid_url = TRUE;
      break;
    }
  }

  if (!$valid_url) {
    form_set_error('field_twitter_url', t('Provided URL is not a valid twitter URL.'));
  }
}

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
  $uri = resources_resource_twitter_parse($form_state['values']['field_twitter_url'][$language][0]['url']);
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
function resources_resource_twitter_parse($url) {
  // Find out what URL we're provided with.
  if (preg_match(TWITTER_STATUS_URL_PATTERN, $url, $matches)) {
    $params = array(
      'u' => $matches[1],
      's' => $matches[2],
    );
  }
  elseif (preg_match(TWITTER_COLLECTION_URL_PATTERN, $url, $matches)) {
    $params = array(
      'u' => $matches[1],
      't' => $matches[2],
    );
  }
  elseif (preg_match(TWITTER_USER_TIMELINE_URL_PATTERN, $url, $matches)) {
    $params['u'] = ltrim($matches[1], '@');
  }
  elseif (preg_match(TWITTER_WIDGET_ID_URL_PATTERN, $url, $matches)) {
    $params['w'] = $matches[1];
  }

  // In case we don't have a widget id, set the default.
  if (empty($params['w'])) {
    $params['w'] = variable_get('resources_default_resource_twitter_widget_id', '');
  }

  // We should have some params by now, if not, something is wrong with validation.
  if (empty($params)) {
    return FALSE;
  }

  // @todo get sheme from resources_info.
  $scheme = 'twitter://';
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
