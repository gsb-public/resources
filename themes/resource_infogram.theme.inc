<?php

/**
 * @file resources/themes/resource_infogram.theme.inc
 *
 * Theme and preprocess functions for Resource: Infogram.
 */

/**
 * Preprocess function for theme('resource_infogram').
 */
function resources_preprocess_resource_infogram(&$variables) {
  $fred = 1;
  $variables['code'] = $variables['options']['code'];
}