<?php
// $Id$

function lab_migration_download_solution_file()
{
  $solution_file_id = arg(3);
  $root_path = lab_migration_path();

 // $solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE id = %d LIMIT 1", $solution_file_id);
  $query = db_select('lab_migration_solution_files');
          $query->fields('lab_migration_solution_files');
          $query->condition('id', $solution_file_id);
          $query->range(0, 1);
          $solution_files_q = $query->execute();
          $solution_file_data = $solution_files_q->fetchObject();
  header('Content-Type:application/zip '/* . $solution_file_data->filemime*/);
  header('Content-disposition: attachment; filename="' . str_replace(' ','_',($solution_file_data->filename)) . '"');
  header('Content-Length: ' . filesize($root_path . $solution_file_data->filepath));
ob_clean();

  readfile($root_path . $solution_file_data->filepath);
}

function lab_migration_download_pdf_file()
{
  $solution_file_id = arg(3);
  $root_path = lab_migration_path();

 // $solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE id = %d LIMIT 1", $solution_file_id);
  $query = db_select('lab_migration_solution_files');
          $query->fields('lab_migration_solution_files');
          $query->condition('id', $solution_file_id);
          $query->range(0, 1);
          $solution_files_q = $query->execute();
          $solution_file_data = $solution_files_q->fetchObject();
	  $pdfname=substr($solution_file_data->pdfpath, strrpos($solution_file_data->pdfpath, '/') + 1);
  header('Content-Type: application/pdf');
  header('Content-disposition: attachment; filename="' . str_replace(' ','_',($pdfname)) . '"');
  header('Content-Length: ' . filesize($root_path . $solution_file_data->pdfpath));
  readfile($root_path . $solution_file_data->pdfpath);
}

function lab_migration_download_sample_code()
{
  $proposal_id = arg(3);
  $root_path = lab_migration_samplecode_path();

	$query = db_select('lab_migration_proposal');
	$query->fields('lab_migration_proposal');
	$query->condition('id', $proposal_id);
	$query->range(0, 1);
	$result = $query->execute();

	$example_file_data =$result->fetchObject();
	$samplecodename=substr($example_file_data->samplefilepath, strrpos($example_file_data->samplefilepath, '/') + 1);
  header('Content-Type: application/zip');
  header('Content-disposition: attachment; filename="'.$samplecodename.'"');
  header('Content-Length: ' . filesize($root_path . $example_file_data->samplefilepath));
  ob_clean();
  readfile($root_path . $example_file_data->samplefilepath);

}


function lab_migration_download_dependency_file()
{
  $dependency_file_id = arg(3);
  $root_path = lab_migration_path();

  //$dependency_file_q = db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $dependency_file_id);
  $query = db_select('lab_migration_dependency_files');
  $query->fields('lab_migration_dependency_files');
  $query->condition('id', $dependency_file_id);
  $query->range(0, 1);
  $dependency_files_q = $query->execute();
  $dependency_file_data = $dependency_files_q->fetchObject();
  header('Content-Type: ' . $dependency_file_data->filemime);
  header('Content-disposition: attachment; filename="' . str_replace(' ','_',($dependency_file_data->filename)) . '"');
  header('Content-Length: ' . filesize($root_path . $dependency_file_data->filepath));
  readfile($root_path . $dependency_file_data->filepath);
}

function lab_migration_download_solution()
{
  $solution_id = arg(3);
  $root_path = lab_migration_path();

  /* get solution data */
  //$solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE id = %d", $solution_id);
  $query = db_select('lab_migration_solution');
            $query->fields('lab_migration_solution');
            $query->condition('id', $solution_id);
            $solution_q = $query->execute();
  $solution_data = $solution_q->fetchObject();
  //$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d", $solution_data->experiment_id);
  $query = db_select('lab_migration_experiment');
            $query->fields('lab_migration_experiment');
            $query->condition('id', $solution_data->experiment_id);
            $experiment_q = $query->execute();
  $experiment_data = $experiment_q->fetchObject();
  //$solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $solution_id);
  $query = db_select('lab_migration_solution_files');
              $query->fields('lab_migration_solution_files');
              $query->condition('solution_id', $solution_id );
              $solution_files_q = $query->execute();
  //$solution_dependency_files_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $solution_id);
  $query = db_select('lab_migration_solution_dependency');
            $query->fields('lab_migration_solution_dependency');
            $query->condition('solution_id', $solution_id);
            $solution_dependency_files_q = $query->execute();

  $CODE_PATH = 'CODE' . $solution_data->code_number . '/';

  /* zip filename */
  $zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';

  /* creating zip archive on the server */
  $zip = new ZipArchive;
  $zip->open($zip_filename, ZipArchive::CREATE);

  while ($solution_files_row = $solution_files_q->fetchObject())
  {
      $zip->addFile($root_path . $solution_files_row->filepath, $CODE_PATH . str_replace(' ','_',($solution_files_row->filename)));
	if(strlen($solution_files_row->pdfpath)>=5){
	$pdfname=substr($solution_files_row->pdfpath, strrpos($solution_files_row->pdfpath, '/') + 1);
	$zip->addFile($root_path . $solution_files_row->pdfpath, $CODE_PATH . str_replace(' ','_',($pdfname)));
	}
  }
  /* dependency files */
  while ($solution_dependency_files_row = $solution_dependency_files_q->fetchObject())
  {
    //$dependency_file_data = (db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $solution_dependency_files_row->dependency_id))->fetchObject();
    $query = db_select('lab_migration_dependency_files');
              $query->fields('lab_migration_dependency_files');
              $query->condition('id', $solution_dependency_files_row->dependency_id);
              $query->range(0, 1);
              $dependency_file_data = $query->execute()->fetchObject();

    if ($dependency_file_data)
      $zip->addFile($root_path . $dependency_file_data->filepath, $CODE_PATH . 'DEPENDENCIES/' . str_replace(' ','_',($dependency_file_data->filename)));
  }
  $zip_file_count = $zip->numFiles;
  $zip->close();

  if ($zip_file_count > 0)
  {
    /* download zip file */
    header('Content-Type: application/zip');
    header('Content-disposition: attachment; filename="CODE' . $solution_data->code_number . '.zip"');
    header('Content-Length: ' . filesize($zip_filename));
    ob_clean();
    //flush();
    readfile($zip_filename);
    unlink($zip_filename);
  } else {
    drupal_set_message("There are no files in this solutions to download", 'error');
    drupal_goto('lab_migration_run');
  }
}

function lab_migration_download_experiment()
{
  $experiment_id = arg(3);
  $root_path = lab_migration_path();

  /* get solution data */
  //$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d", $experiment_id);
  $query = db_select('lab_migration_experiment');
           $query->fields('lab_migration_experiment');
           $query->condition('id', $experiment_id);
           $experiment_q = $query->execute();

  
   $experiment_data = $experiment_q->fetchObject();
  $EXP_PATH = 'EXP' . $experiment_data->number . '/';

  /* zip filename */
  $zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';

  /* creating zip archive on the server */
  $zip = new ZipArchive;
  $zip->open($zip_filename, ZipArchive::CREATE);

  //$solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d AND approval_status = 1", $experiment_id);
                  $query = db_select('lab_migration_solution');
                  $query->fields('lab_migration_solution');
                  $query->condition('experiment_id', $experiment_id);
                  $query->condition('approval_status', 1);
                  $solution_q = $query->execute();
  while ($solution_row = $solution_q->fetchObject())
  {
    $CODE_PATH = 'CODE' . $solution_row->code_number . '/';
   // $solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $solution_row->id);
    $query = db_select('lab_migration_solution_files');
                $query->fields('lab_migration_solution_files');
                $query->condition('solution_id', $solution_row->id);
                $solution_files_q = $query->execute();
   // $solution_dependency_files_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $solution_row->id);
    $query = db_select('lab_migration_solution_dependency');
              $query->fields('lab_migration_solution_dependency');
              $query->condition('solution_id', $solution_row->id);
              $solution_dependency_files_q = $query->execute();
    while ($solution_files_row = $solution_files_q->fetchObject())
    {
      $zip->addFile($root_path . $solution_files_row->filepath, $EXP_PATH . $CODE_PATH . str_replace(' ','_',($solution_files_row->filename)));
	if(strlen($solution_files_row->pdfpath)>=5){
	$pdfname=substr($solution_files_row->pdfpath, strrpos($solution_files_row->pdfpath, '/') + 1);
	$zip->addFile($root_path . $solution_files_row->pdfpath, $EXP_PATH . $CODE_PATH . str_replace(' ','_',($pdfname)));
	}
    }
    /* dependency files */
    while ($solution_dependency_files_row = $solution_dependency_files_q->fetchObject())
    {
      //$dependency_file_data = (db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $solution_dependency_files_row->dependency_id))->fetchObject();
      $query = db_select('lab_migration_dependency_files');
                $query->fields('lab_migration_dependency_files');
                $query->condition('id', $solution_dependency_files_row->dependency_id);
                $query->range(0, 1);
                $dependency_file_data = $query->execute()->fetchObject();
      if ($dependency_file_data)
        $zip->addFile($root_path . $dependency_file_data->filepath, $EXP_PATH . $CODE_PATH . 'DEPENDENCIES/' . str_replace(' ','_',($dependency_file_data->filename)));
    }
  }
  $zip_file_count = $zip->numFiles;
  $zip->close();

  if ($zip_file_count > 0)
  {
    /* download zip file */
    header('Content-Type: application/zip');
    header('Content-disposition: attachment; filename="EXP' . $experiment_data->number . '.zip"');
    header('Content-Length: ' . filesize($zip_filename));
    ob_clean();
    //flush();
    readfile($zip_filename);
    unlink($zip_filename);
  } else {
    drupal_set_message("There are no solutions in this experiment to download", 'error');
    drupal_goto('lab_migration_run');
  }
}

function lab_migration_download_lab()
{
  global $user;
  $lab_id = arg(3);
  
  $root_path = lab_migration_path();

  /* get solution data */
  //$lab_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $lab_id);
  $query = db_select('lab_migration_proposal');
            $query->fields('lab_migration_proposal');
            $query->condition('id', $lab_id);
            $lab_q = $query->execute();
  $lab_data = $lab_q->fetchObject();
  $LAB_PATH = $lab_data->lab_title . '/';

  

  /* zip filename */
  $zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';

  /* creating zip archive on the server */
  $zip = new ZipArchive();
  $zip->open($zip_filename, ZipArchive::CREATE);

  //$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d", $lab_id);
  $query = db_select('lab_migration_experiment');
              $query->fields('lab_migration_experiment');
              $query->condition('proposal_id', $lab_id);
              $experiment_q = $query->execute();
  while ($experiment_row = $experiment_q->fetchObject())
  {
    $EXP_PATH = 'EXP' . $experiment_row->number . '/';
    //$solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d AND approval_status = 1", $experiment_row->id);
    $query = db_select('lab_migration_solution');
              $query->fields('lab_migration_solution');
              $query->condition('experiment_id', $experiment_row->id);
              $query->condition('approval_status', 1);
              $solution_q = $query->execute();
    while ($solution_row = $solution_q->fetchObject())
    {
      $CODE_PATH = 'CODE' . $solution_row->code_number . '/';
      //$solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $solution_row->id);
      $query = db_select('lab_migration_solution_files');
                $query->fields('lab_migration_solution_files');
                $query->condition('solution_id', $solution_row->id);
                $solution_files_q = $query->execute();
      //$solution_dependency_files_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $solution_row->id);
      
      $query = db_select('lab_migration_solution_dependency');
                  $query->fields('lab_migration_solution_dependency');
                  $query->condition('solution_id', $solution_row->id);
                  $solution_dependency_files_q = $query->execute();
      while ($solution_files_row = $solution_files_q->fetchObject())
      {
        $zip->addFile($root_path . $solution_files_row->filepath, $LAB_PATH . $EXP_PATH . $CODE_PATH . str_replace(' ','_',($solution_files_row->filename)));
       if(strlen($solution_files_row->pdfpath)>=5){
	$pdfname=substr($solution_files_row->pdfpath, strrpos($solution_files_row->pdfpath, '/') + 1);
	$zip->addFile($root_path . $solution_files_row->pdfpath, $LAB_PATH . $EXP_PATH . $CODE_PATH . str_replace(' ','_',($pdfname)));
	}
      }
      
      // die;
      /* dependency files */
      while ($solution_dependency_files_row = $solution_dependency_files_q->fetchObject())
      {
        //$dependency_file_data = (db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $solution_dependency_files_row->dependency_id))->fetchObject();
        $query = db_select('lab_migration_dependency_files');
                  $query->fields('lab_migration_dependency_files');
                  $query->condition('id', $solution_dependency_files_row->dependency_id);
                  $query->range(0, 1);
                  $dependency_file_data = $query->execute()->fetchObject();
        if ($dependency_file_data)
          $zip->addFile($root_path . $dependency_file_data->filepath, $LAB_PATH . $EXP_PATH . $CODE_PATH . 'DEPENDENCIES/' . str_replace(' ','_',($dependency_file_data->filename)));
      }
    }
  }
  $zip_file_count = $zip->numFiles;
  $zip->close();
    
  if ($zip_file_count > 0)
  {
     if($user->uid) {
    	/* download zip file */
    	header('Content-Type: application/zip');
    	header('Content-disposition: attachment; filename="' .str_replace(' ', '_', $lab_data->lab_title) . '.zip"');
    	header('Content-Length: ' . filesize($zip_filename));
    	ob_clean();
    	//flush();
    	readfile($zip_filename);
    	unlink($zip_filename);
     } else {
	header('Content-Type: application/zip');
        header('Content-disposition: attachment; filename="' .str_replace(' ', '_', $lab_data->lab_title) . '.zip"');
        header('Content-Length: ' . filesize($zip_filename));
        header("Content-Transfer-Encoding: binary");
	header('Expires: 0');
	header('Pragma: no-cache');
	ob_end_flush();
	ob_clean();
	flush();
        readfile($zip_filename);
        unlink($zip_filename);
     }
  } else {
    drupal_set_message("There are no solutions in this Lab to download", 'error');
    drupal_goto('lab_migration_run');
  }
}

