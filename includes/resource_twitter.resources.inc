<?php

// Define patters of supported twitter URLs.
// @TODO: Move constants to handler class.
const TWITTER_STATUS_URL_PATTERN = '@twitter\.com/([^\/]+)/status/([0-9]*)$@i';
const TWITTER_COLLECTION_URL_PATTERN = '@twitter\.com/([^\/]+)/timelines/([0-9]*)$@i';
const TWITTER_USER_TIMELINE_URL_PATTERN = '@twitter\.com/([^/\?#]*)$@i';
const TWITTER_WIDGET_ID_URL_PATTERN = '@twitter.com/settings/widgets/([0-9]+)/edit$@i';

/**
 * Parse the twitter url.
 *
 * @todo: Move to handler class.
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

  // We should have some params by now, if not, something is wrong with validation.
  if (empty($params)) {
    throw new ResourcesValidationException('Provided URL is not a valid twitter URL.');
    return FALSE;
  }

  // In case we don't have a widget id, set the default.
  if (empty($params['w'])) {
    $params['w'] = variable_get('resources_default_resource_twitter_widget_id', '');
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
