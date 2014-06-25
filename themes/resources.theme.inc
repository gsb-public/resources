<?php


/**
 * Theme function that displays an icon for resources.
 */
function theme_resources_resource_icon($variables) {
  $file = $variables['file'];

  $info = resources_get_info_by_file($file);
  if (!isset($info['icon'])) {
    return;
  }

  $icon = variable_get('resources_icons_directory', 'public://resources-assets') . '/' . $info['icon'];

  $variables['path'] = $icon;

  // theme_image() requires the 'alt' attribute passed as its own variable.
  // @see http://drupal.org/node/999338
  if (!isset($variables['alt']) && isset($variables['attributes']['alt'])) {
    $variables['alt'] = $variables['attributes']['alt'];
  }

  // Add image height and width for the image theme functions.
  if ($info = image_get_info($icon)) {
    $variables += $info;
  }

  if (!empty($variables['style_name'])) {
    $output = theme('image_style', $variables);
  }
  else {
    $output = theme('image', $variables);
  }
  return $output;
}
