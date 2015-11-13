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

  // Load the soundcloud information.
  $soundcloud = new ResourcesSoundCloudStreamWrapper;
  $soundcloud->setUri($variables['uri']);

  // Get the parameters.
  $parts = $soundcloud->get_parameters();

  // Set some variables from the soundcloud parameters.
  $variables['track_id'] = $parts['id'];
  $variables['user_id'] = $parts['u'];

  $options = array(
    'color',
    'auto_play',
    'hide_related',
    'show_comments',
    'show_user',
    'show_reposts',
    'visual',
    'inverse'
  );
  $parameters = '';
  foreach($options as $option) {
    if (!isset($variables['options'][$option])) {
      continue;
    }
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
