<?php

/**
 *  @file resources/includes/ResourcesSoundCloudStreamWrapper.inc
 *
 *  Create a SoundCloud Stream Wrapper class for the Media/Resource module.
 */

/**
 *  Create an instance like this:
 *  $soundcloud = new ResourcesSoundCloudStreamWrapper('soundcloud://v/[video-code]');
 */
class ResourcesSoundCloudStreamWrapper extends ResourcesReadOnlyStreamWrapper {

  // Overrides $base_url defined in MediaReadOnlyStreamWrapper.
  protected $base_url = 'http://soundcloud.com/watch';

  /**
   * Returns a url in the format "http://soundcloud.com/watch?v=qsPQN4MiTeE".
   *
   * Overrides interpolateUrl() defined in ResourcesReadOnlyStreamWrapper.
   * This is an exact copy of the function in ResourcesReadOnlyStreamWrapper,
   * here in case that example is redefined or removed.
   */
  function interpolateUrl() {
    if ($parameters = $this->get_parameters()) {
      return $this->base_url . '?' . http_build_query($parameters);
    }
  }

  static function getMimeType($uri, $mapping = NULL) {
    return 'audio/soundcloud';
  }

  function getTarget($f) {
    return FALSE;
  }

  function getOriginalThumbnailPath() {
    $parts = $this->get_parameters();
    $uri = 'soundcloud://u/' . $parts['u'] . '/id/' . $parts['id'] . '/hash/' . $parts['hash'];
    $file_object = file_uri_to_object($uri);
    $track_url = $file_object->field_soundcloud_url['und'][0]['value'];
    $track_info = gsb_soundcloud_get_track_info($track_url);
    return $track_info->artwork_url;
  }

  function getLocalThumbnailPath() {
    $parts = $this->get_parameters();
    // There's no need to hide thumbnails, always use the public system rather
    // than file_default_scheme().
    $local_path = 'public://resources-soundcloud/' . check_plain($parts['id']) . '.jpg';
    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      $response = drupal_http_request($this->getOriginalThumbnailPath());
      if (!isset($response->error)) {
        file_unmanaged_save_data($response->data, $local_path, TRUE);
      }
      else {
        @copy($this->getOriginalThumbnailPath(), $local_path);
      }
    }
    return $local_path;
  }
}
