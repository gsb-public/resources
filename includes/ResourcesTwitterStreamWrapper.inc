<?php

/**
 *  @file resources/includes/ResourcesTwitterStreamWrapper.inc
 *
 *  Create a Twitter Tweet/feed Stream Wrapper class for the Resources module.
 */

/**
 *  Create an instance like this:
 *  $tweet = new ResourcesTweetStreamWrapper('tweet://u/[user-id]/s/[status-id]');
 */
class ResourcesTwitterStreamWrapper extends ResourcesReadOnlyStreamWrapper {

  // Overrides $base_url defined in ResourcesReadOnlyStreamWrapper.
  protected $base_url = 'http://twitter.com';
  protected $parameters = array('q' => NULL, 's' => NULL,  't' => NULL, 'u' => NULL, 'w' => NULL);

  /**
   * Returns a twitter url in the format.
   *
   * Overrides interpolateUrl() defined in ResourcesReadOnlyStreamWrapper.
   */
  function interpolateUrl() {
    if ($params = $this->get_parameters()) {
      $url = $this->base_url . '/';

      if (!empty($params['u'])) {
        $url .= $params['u'];
        if (!empty($params['s'])) {
          $url .= '/status/' . $params['s'];
        }
        elseif (!empty($params['t'])) {
          $url .= '/timelines/' . $params['t'];
        }
      }
      // If username is not set we probably deal with Widget URL.
      else {
        // Provide a placeholder URL. @see https://dev.twitter.com/docs/embedded-timelines#search
        $url .= 'twitterapi';
      }

      return $url;
    }
  }

  static function getMimeType($uri, $mapping = NULL) {
    return 'application/twitter';
  }

  function getTarget($f) {
    return FALSE;
  }
}
