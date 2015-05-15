<?php

/**
 * Base ResourcesResourceBaseHandler class.
 *
 * Resource types handlers should implement this class to parse, validate and save resources.
 */
class ResourcesSoundCloudHandler extends ResourcesResourceBaseHandler {

  public $fileObject;

  /**
   * The constructor for the MediaInternetBaseHandler class. This method is also called
   * from the classes that extend this class and override this method.
   */
  public function __construct(stdClass $file) {
    $this->fileObject = $file;
  }

  public function parse() {
    $wrapper = entity_metadata_wrapper('file', $this->fileObject);
    return resources_resource_soundcloud_parse($wrapper->field_soundcloud_url->value());
  }

  /**
   * Returns a file object which can be used for validation.
   *
   * @return StdClass
   */
  public function getFileObject() {
    $uri = $this->parse();

    // @todo: This might go into a base class.
    if (empty($this->fileObject->uri)) {
      $new_file = file_uri_to_object($uri, TRUE);
      // Merge file defaults.
      $file = (array) $this->fileObject + (array) $new_file;
      $file = (object) $file;
      //$new_file->type = $this->fileObject->type;
      //$new_file->display = $this->fileObject->display;
      $file->filename = $this->fileObject->filename;

      $this->fileObject = $file;
    }
    else {
      $this->fileObject->uri = $uri;
    }

    return $this->fileObject;
  }

  /**
   * If required, implementors can validate the embedCode.
   */
  public function validate() {
  }
}
