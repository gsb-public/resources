<?php

/**
 * @file resources/themes/resource_art19.theme.inc
 *
 * Theme and preprocess functions for Resource: Art19.
 */

drupal_add_js('https://web-player.art19.com/assets/current.js', 'external');
drupal_add_css('https://web-player.art19.com/assets/current.css', 'external');


/**
 * Preprocess function for theme('resource_art19').
 */
function resources_preprocess_resource_art19(&$variables) {
  $variables['code'] = $variables['options']['code'];
}
