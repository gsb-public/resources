<?php

/**
 * Parse the twitter url.
 *
 * @param $url
 * @param null $widget_id
 * @return bool|string
 */
function resources_resource_youtube_parse($embedCode) {
  $patterns = array(
    '@youtube\.com/watch[#\?].*?v=([^"\& ]+)@i',
    '@youtube\.com/embed/([^"\&\? ]+)@i',
    '@youtube\.com/v/([^"\&\? ]+)@i',
    '@youtube\.com/\?v=([^"\& ]+)@i',
    '@youtu\.be/([^"\&\? ]+)@i',
    '@gdata\.youtube\.com/feeds/api/videos/([^"\&\? ]+)@i',
  );
  foreach ($patterns as $pattern) {
    preg_match($pattern, $embedCode, $matches);
    // @TODO: Parse is called often. Refactor so that valid ID is checked
    // when a video is added, but not every time the embedCode is parsed.
    try {
      if (isset($matches[1]) && resources_resource_youtube_valid_id($matches[1])) {
        return file_stream_wrapper_uri_normalize('ryoutube://v/' . $matches[1]);
      }
    }
    catch (Exception $e) {
      watchdog('resource_youtube', 'resources_resource_youtube_parse received the following exception: ' . $e, array(), WATCHDOG_ERROR);
    }
  }
}

/**
 * Check if a YouTube video id is valid.
 *
 * Check against the oembed stream instead of the gdata api site to
 * avoid "yt:quota too_many_recent_calls" errors.
 *
 * @return
 *   Boolean.
 */
function resources_resource_youtube_valid_id($id) {
  $url = 'http://www.youtube.com/oembed?url=http%3A//youtube.com/watch%3Fv%3D'. $id;
  $response = drupal_http_request($url, array('method' => 'HEAD'));
  if ($response->code == 401) {
    throw new Exception("Embedding has been disabled for this video.");
  }
  elseif ($response->code != 200) {
    throw new Exception("The YouTube video ID is invalid or the video was deleted.");
  }
  return TRUE;
}

/**
 * Validation for width and height.
 * @todo: Where is it used?
 */
function _resources_youtube_validate_video_width_and_height($element, &$form_state, $form) {

  // Check if the value is a number with an optional decimal or percentage sign, or "auto".
  if (!empty($element['#value']) && !preg_match('/^(auto|([0-9]*(\.[0-9]+)?%?))$/', $element['#value'])) {
    form_error($element, t("The value entered for @dimension is invalid. Please insert a unitless integer for pixels, a percent, or \"auto\". Note that percent and auto may not function correctly depending on the browser and doctype.", array('@dimension' => $element['#title'])));
  }
}

/**
 * Validation for Js API Origin.
 * @todo: Where is it used?
 */
function _resources_youtube_validate_jsapi_domain($element, &$form_state, $form) {

  // Check if the value is a url with http/s and no trailing directories.
  if (!empty($element['#value']) && !preg_match('/^https?\:\/\/[a-zA-Z0-9\-\.]+\.([a-zA-Z]{2,4}){1,2}$/', $element['#value'])) {
    form_error($element, t('Please insert a valid domain in the format http://www.yourdomain.com'));
  }
}

/**
 * Implements hook_file_default_displays_alter().
 * @todo: It may be implemented by several modules, would be messy if all modules would keep their alters here.
 */
function resources_file_default_displays_alter(&$file_displays) {
  // Video previews should be displayed using a large filetype icon.
  $file_display = new stdClass();
  $file_display->api_version = 1;
  $file_display->name = 'video__default__resources_resource_youtube_video';
  $file_display->weight = 0;
  $file_display->status = TRUE;
  $file_display->settings = array(
    'width' => '640',
    'height' => '390',
    'theme' => 'dark',
    'color' => 'red',
    'autohide' => '2',
    'autoplay' => 0,
    'loop' => 0,
    'showinfo' => 1,
    'modestbranding' => 0,
    'rel' => 1,
    'nocookie' => 0,
    'protocol_specify' => 0,
    'protocol' => 'https:',
    'enablejsapi' => 0,
    'origin' => '',
  );
  $file_displays['video__default__resources_resource_youtube_video'] = $file_display;

  $file_display = new stdClass();
  $file_display->api_version = 1;
  $file_display->name = 'video__preview__resources_resource_youtube_image';
  $file_display->weight = 0;
  $file_display->status = TRUE;
  $file_display->settings = array(
    'image_style' => 'resources_resource_thumbnail',
  );
  $file_displays['video__preview__resources_resource_youtube_image'] = $file_display;

  $file_display = new stdClass();
  $file_display->api_version = 1;
  $file_display->name = 'video__teaser__resources_resource_youtube_video';
  $file_display->weight = 0;
  $file_display->status = TRUE;
  $file_display->settings = array(
    'width' => '560',
    'height' => '340',
    'theme' => 'dark',
    'color' => 'red',
    'autohide' => '2',
    'autoplay' => 0,
    'loop' => 0,
    'showinfo' => 1,
    'modestbranding' => 0,
    'rel' => 1,
    'nocookie' => 0,
    'protocol_specify' => 0,
    'protocol' => 'https:',
    'enablejsapi' => 0,
    'origin' => '',
  );
  $file_displays['video__teaser__resources_resource_youtube_video'] = $file_display;
}
