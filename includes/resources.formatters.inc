<?php

/**
 * Implements hook_file_formatter_info().
 */
function resources_file_formatter_info() {
  // @todo: Should use a getter.
  $twitter_file_type = variable_get('resources_resource_twitter_file_type', '');

  $formatters = array(
    'resources_resource_twitter_widget' => array(
      'label' => t('Twitter Widget'),
      'file types' => array($twitter_file_type),
      'default settings' => array(),
      'view callback' => 'resources_resource_twitter_file_formatter_widget_view',
    ),
    'resources_resource_youtube_video' => array(
      'label' => t('Resources: YouTube Video'),
      'file types' => array('video'),
      'default settings' => array(
        'width' => 640,
        'height' => 390,
        'autohide' => 2,
        'autoplay' => FALSE,
        'color' => 'red',
        'enablejsapi' => FALSE,
        'loop' => FALSE,
        'modestbranding' => FALSE,
        'nocookie' => FALSE,
        'origin' => '',
        'protocol' => 'https:',
        'protocol_specify' => FALSE,
        'rel' => TRUE,
        'showinfo' => TRUE,
        'theme' => 'dark',
      ),
      'view callback' => 'resources_resource_youtube_file_formatter_video_view',
      'settings callback' => 'resources_resource_youtube_file_formatter_video_settings',
    ),
    'resources_resource_youtube_image' => array(
      'label' => t('Resources: YouTube Preview Image'),
      'file types' => array('video'),
      'default settings' => array(
        'image_style' => '',
      ),
      'view callback' => 'resources_resource_youtube_file_formatter_image_view',
      'settings callback' => 'resources_resource_youtube_file_formatter_image_settings',
    ),
    'resources_file_fields' => array(
      'label' => t('Resources: File fields only'),
      'view callback' => 'resources_file_fields_view',
    ),
    'resources_resource_icon' => array(
      'label' => t('Resources: Resource icon'),
      'default settings' => array(
        'image_style' => '',
      ),
      'view callback' => 'resources_resource_icon_view',
      'settings callback' => 'resources_resource_icon_settings',
    )
  );

  return $formatters;
}


/**
 * Implements hook_file_formatter_FORMATTER_view().
 */
function resources_resource_twitter_file_formatter_widget_view($file, $display, $langcode) {
  $twitter = new ResourcesTwitterStreamWrapper();
  $twitter->setUri($file->uri);
  $params = $twitter->get_parameters();
  $href = $twitter->interpolateUrl();
  $output = '';

  if (!$href) {
    return FALSE;
  }

  $options = array(
    'attributes' => array(
      'class' => array(
        'twitter-timeline',
      ),
      'data-dnt' => 'true',
      'data-widget-id' => $params['w'],
    ),
  );

  if (!is_null($params['s'])) {
    $url = 'https://api.twitter.com/1/statuses/oembed.json?id=' . $params['s'];
    $response = drupal_http_request($url);
    if (!isset($response->error)) {
      $tweet_info = drupal_json_decode($response->data);
      $output = $tweet_info['html'];
    }
  }
  else {
    if (!is_null($params['t'])) {
      $options['attributes']['data-custom-timeline-id'] = $params['t'];
    }
    elseif (!is_null($params['u'])) {
      $options['attributes']['data-screen-name'] = $params['u'];
    }

    $output = l($file->filename, $href, $options);
    $output .= '<script>!function(d,s,id){var js,fjs=d.getElementsByTagName(s)[0],p=/^http:/.test(d.location)?\'http\':\'https\';if(!d.getElementById(id)){js=d.createElement(s);js.id=id;js.src=p+"://platform.twitter.com/widgets.js";fjs.parentNode.insertBefore(js,fjs);}}(document,"script","twitter-wjs");</script>';
  }

  $element = array(
    '#markup' => $output,
  );

  return $element;
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
