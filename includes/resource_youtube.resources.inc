<?php

/**
 * Callback for when the file is saved.
 *
 * @param $file
 * @param $form
 * @param $form_state
 * @return bool|object
 */
function resources_resource_youtube_presave($file, $form, $form_state) {
  $language = $form['field_youtube_embed_code']['#language'];
  $uri = resources_resource_youtube_parse($form_state['values']['field_youtube_embed_code'][$language][0]['value']);

  if (empty($file->uri)) {
    // We pass second argument as FALSE to always create new files.
    $new_file = file_uri_to_object($uri, FALSE);
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
    if (isset($matches[1]) && resources_resource_youtube_valid_id($matches[1])) {
      return file_stream_wrapper_uri_normalize('ryoutube://v/' . $matches[1]);
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
 * Implements hook_file_formatter_FORMATTER_view().
 */
function resources_resource_youtube_file_formatter_video_view($file, $display, $langcode) {
  $scheme = file_uri_scheme($file->uri);
  // WYSIWYG does not yet support video inside a running editor instance.
  if ($scheme == 'ryoutube' && empty($file->override['wysiwyg'])) {
    $element = array(
      '#theme' => 'resource_youtube_video',
      '#uri' => $file->uri,
      '#options' => array(),
    );

    // Fake a default for attributes so the ternary doesn't choke.
    $display['settings']['attributes'] = array();

    foreach (array('width', 'height', 'autohide', 'autoplay', 'color', 'enablejsapi', 'loop', 'modestbranding', 'nocookie', 'origin', 'protocol', 'protocol_specify', 'rel', 'showinfo', 'theme', 'attributes') as $setting) {
      $element['#options'][$setting] = isset($file->override[$setting]) ? $file->override[$setting] : $display['settings'][$setting];
    }
    return $element;
  }
}

/**
 * Implements hook_file_formatter_FORMATTER_settings().
 */
function resources_resource_youtube_file_formatter_video_settings($form, &$form_state, $settings) {
  $element = array();

  $element['width'] = array(
    '#title' => t('Width'),
    '#type' => 'textfield',
    '#default_value' => $settings['width'],
    '#element_validate' => array('_resources_youtube_validate_video_width_and_height'),
  );
  $element['height'] = array(
    '#title' => t('Height'),
    '#type' => 'textfield',
    '#default_value' => $settings['height'],
    '#element_validate' => array('_resources_youtube_validate_video_width_and_height'),
  );

  // @see https://developers.google.com/youtube/player_parameters#Parameters

  // Multiple options.
  $element['theme'] = array(
    '#title' => t('Player theme'),
    '#type' => 'radios',
    '#options' => array(
      'dark' => t('Dark'),
      'light' => t('Light'),
    ),
    '#default_value' => $settings['theme'],
  );
  $element['color'] = array(
    '#title' => t('Progress bar color'),
    '#type' => 'radios',
    '#options' => array(
      'red' => t('Red'),
      'white' => t('White'),
    ),
    '#default_value' => $settings['color'],
  );
  $element['autohide'] = array(
    '#title' => t('Controls during playback'),
    '#type' => 'radios',
    '#options' => array(
      '0' => t('Keep progress bar and player controls on screen while playing'),
      '2' => t('Hide progress bar while playing'),
      '1' => t('Hide progress bar and player controls'),
    ),
    '#default_value' => $settings['autohide'],
  );

  // Single Options.
  $element['autoplay'] = array(
    '#title' => t('Autoplay video on load'),
    '#type' => 'checkbox',
    '#default_value' => $settings['autoplay'],
  );
  $element['loop'] = array(
    '#title' => t('Loop video'),
    '#type' => 'checkbox',
    '#default_value' => $settings['loop'],
  );

  // Note: make sure the positive/negitive language lines up with option
  // processing in resources_resource_youtube.theme.inc.
  $element['showinfo'] = array(
    '#title' => t('Display video title and uploader'),
    '#type' => 'checkbox',
    '#default_value' => $settings['showinfo'],
  );
  $element['modestbranding'] = array(
    '#title' => t('Remove YouTube logo from the control bar'),
    '#type' => 'checkbox',
    '#default_value' => $settings['modestbranding'],
  );
  $element['rel'] = array(
    '#title' => t('Show related videos when playback ends'),
    '#type' => 'checkbox',
    '#default_value' => $settings['rel'],
  );
  $element['nocookie'] = array(
    '#title' => t('Use privacy enhanced (no cookie) mode'),
    '#type' => 'checkbox',
    '#default_value' => $settings['nocookie'],
  );
  $element['protocol_specify'] = array(
    '#title' => t('Specify an http protocol'),
    '#type' => 'checkbox',
    '#default_value' => $settings['protocol_specify'],
    '#description' => t('An explicit protocol may be neccesary for videos embedded in RSS feeds and emails. If no protocol is specified, iframes will be protocol relative.'),
  );
  $element['protocol'] = array(
    '#title' => t('Iframe protocol'),
    '#type' => 'radios',
    '#default_value' => $settings['protocol'],
    '#options' => array(
      'http:' => 'http://',
      'https:' => 'https://',
    ),
    '#states' => array(
      'invisible' => array(
        ':input[name="displays[resources_resource_youtube_video][settings][protocol_specify]"]' => array('checked' => FALSE),
      ),
    ),
  );

  // JS api.
  $element['enablejsapi'] = array(
    '#title' => t('Enable the <a href="https://developers.google.com/youtube/js_api_reference">Javascript API</a>'),
    '#type' => 'checkbox',
    '#default_value' => $settings['enablejsapi'],
    '#description' => 'An id in the format <code>resources_resource-youtube-{video_id}</code> will be added to each iframe.',
  );
  $element['origin'] = array(
    '#title' => t('Origin'),
    '#type' => 'textfield',
    '#description' => t('If the Javascript API is enabled, enter your site\'s domain for added security.'),
    '#default_value' => $settings['origin'],
    '#states' => array(
      'invisible' => array(
        ':input[name="displays[resources_resource_youtube_video][settings][enablejsapi]"]' => array('checked' => FALSE),
      ),
    ),
    '#element_validate' => array('_resources_youtube_validate_jsapi_domain'),
  );

  return $element;
}

/**
 * Validation for width and height.
 */
function _resources_youtube_validate_video_width_and_height($element, &$form_state, $form) {

  // Check if the value is a number with an optional decimal or percentage sign, or "auto".
  if (!empty($element['#value']) && !preg_match('/^(auto|([0-9]*(\.[0-9]+)?%?))$/', $element['#value'])) {
    form_error($element, t("The value entered for @dimension is invalid. Please insert a unitless integer for pixels, a percent, or \"auto\". Note that percent and auto may not function correctly depending on the browser and doctype.", array('@dimension' => $element['#title'])));
  }
}

/**
 * Validation for Js API Origin.
 */
function _resources_youtube_validate_jsapi_domain($element, &$form_state, $form) {

  // Check if the value is a url with http/s and no trailing directories.
  if (!empty($element['#value']) && !preg_match('/^https?\:\/\/[a-zA-Z0-9\-\.]+\.([a-zA-Z]{2,4}){1,2}$/', $element['#value'])) {
    form_error($element, t('Please insert a valid domain in the format http://www.yourdomain.com'));
  }
}

/**
 * Implements hook_file_formatter_FORMATTER_view().
 */
function resources_resource_youtube_file_formatter_image_view($file, $display, $langcode) {
  $scheme = file_uri_scheme($file->uri);
  if ($scheme == 'ryoutube') {
    $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
    $image_style = $display['settings']['image_style'];
    $valid_image_styles = image_style_options(FALSE);
    // @TODO: If autosubmit is removed and we allow view modes that insert
    // images in the WYSIWYG, add file->overrides handling.
    if (empty($image_style) || !isset($valid_image_styles[$image_style])) {
      $element = array(
        '#theme' => 'image',
        '#path' => $wrapper->getOriginalThumbnailPath(),
        '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
      );
    }
    else {
      $element = array(
        '#theme' => 'image_style',
        '#style_name' => $image_style,
        '#path' => $wrapper->getLocalThumbnailPath(),
        '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
      );
    }

    return $element;
  }
}

/**
 * Implements hook_file_formatter_FORMATTER_settings().
 */
function resources_resource_youtube_file_formatter_image_settings($form, &$form_state, $settings) {
  $element = array();
  $element['image_style'] = array(
    '#title' => t('Image style'),
    '#type' => 'select',
    '#options' => image_style_options(FALSE),
    '#default_value' => $settings['image_style'],
    '#empty_option' => t('None (original image)'),
  );
  return $element;
}

/**
 * Implements hook_file_default_displays_alter().
 */
function resources_resource_youtube_file_default_displays_alter(&$file_displays) {
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
