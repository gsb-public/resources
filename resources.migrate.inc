<?php

/**
 * Implements hook_migrate_api().
 */
function resources_migrate_api() {
  $api = array(
    'api' => MIGRATE_API_VERSION,
    'destination handlers' => array(
      'ResourcesResourcesHandler',
    ),
  );
  return $api;
}

/**
 * Support Resources attached to nodes.
 *  - Adds a field that accepts a nid and will attach imported file to that node.
 */
class ResourcesResourcesHandler extends MigrateDestinationHandler {
  public function __construct() {
    $this->registerTypes(array('file'));
  }

  public function fields($entity_type, $bundle) {
    $fields = array(
      'resource_host_node' => t('Resources are attached to nodes. Pass host node nid here to attach this file to a node.'),
    );

    return $fields;
  }

  /**
   * Prepares $entity before saving.
   *  - Not all resources require a URI, ResourcesEntityHandler generates a URI before saving,
   *    other resources might generate a URI from other fields. Set value to a fake_value if it's not set.
   *
   * @param $entity
   * @param stdClass $row
   */
  public function prepare($entity, stdClass $row) {
    if (!empty($entity->resource_type) && resources_get_info($entity->resource_type)) {
      if (empty($entity->value)) {
        $entity->value = 'fake_value';
      }
    }
  }

  /**
   * Complete callback executed after entity is saved.
   *  - Add file usage data in order to create a reference between host node and resource file.
   */
  public function complete($entity, stdClass $row) {
    if (module_exists('resources') && !empty($entity->resource_host_node)) {
      file_usage_add($entity, 'resources', 'node', $entity->resource_host_node);
    }
  }
}

/**
 * Class for creating Resources file entities to be used with Migrate module.
 */
class ResourcesResourcesFile implements MigrateFileInterface {

  public $fileObject;
  public $resourceTypeHandler;

  public function __construct($arguments = array(), $default_file = NULL) {
    $this->fileObject = $default_file;
    // @TODO: Make sure resource_type is set.
    $this->resourceTypeHandler = resources_get_type_handler($this->fileObject->resource_type, $this->fileObject);
  }

  public static function fields() {
    return array(
      'resource_type' => t('Specify a resource type.'),
    );
  }

  /**
   * Implementation of MigrateFileInterface::processFiles().
   *
   * @param $value
   *  The URI or local filespec of a file to be imported.
   * @param $owner
   *  User ID (uid) to be the owner of the file.
   * @return object
   *  The file entity being created or referenced.
   */
  public function processFile($value, $owner) {
    try {
      // Providers decide if they need to save locally or somewhere else.
      // This method returns a file object
      $file = $this->resourceTypeHandler->save();
    }
    catch (Exception $e) {
      throw new MigrateException($e->getMessage());
    }

    if (is_object($file)) {
      return $file;
    }
    else {
      return FALSE;
    }
  }
}
