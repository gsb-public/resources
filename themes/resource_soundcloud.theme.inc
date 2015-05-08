<?php

/**
 * @file resources/themes/resource_soundcloud.theme.inc
 *
 * Theme and preprocess functions for Resource: SoundCloud.
 */

/**
 * Preprocess function for theme('resource_soundcloud').
 */
function resources_preprocess_resource_soundcloud(&$variables) {

  // Make the file object available.
  $file_object = file_uri_to_object($variables['uri']);

  $track_url = $file_object->field_soundcloud_url['und'][0]['value'];
  $track_info = soundcloud_api_get_track_info($track_url);

  $variables['track_id'] = $track_info->id;

  $options = array(
    'color',
    'auto_play',
    'hide_related',
    'show_comments',
    'show_user',
    'show_reposts',
    'visual'
  );
  $parameters = '';
  foreach($options as $option) {
    if ($parameters != '') {
      $parameters .= '&amp;';
    }
    $option_value = !empty($variables['options'][$option]) ? 'true' : 'false';
    if ($option == 'color' ) {
      $option_value = $variables['options'][$option];
    }
    if ($option_value != null) {
      $parameters .= $option . '=' . $option_value;
    }
  }
  $variables['parameters'] = $parameters;

}
