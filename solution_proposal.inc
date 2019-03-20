<?php
// $Id$

function lab_migration_proposal_open()
{
  global $user;

  /* get open proposal list */
  $proposal_rows = array();
  //$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE approval_status = 1 AND solution_provider_uid = 0");
  $query = db_select('lab_migration_proposal');
            $query->fields('lab_migration_proposal');
            $query->condition('approval_status', 1);
            $query->condition('solution_provider_uid', 0);
            $proposal_q = $query->execute();
  while ($proposal_data = $proposal_q->fetchObject())
  {
    $proposal_rows[] = array(l($proposal_data->lab_title, 'lab_migration/show_proposal/' . $proposal_data->id), l('Apply', 'lab_migration/show_proposal/' . $proposal_data->id));
  }

  $proposal_header = array('Title of the Lab', 'Actions');
  $return_html = theme('table', array('header' => $proposal_header, 'rows' => $proposal_rows));
  //$return_html = theme_table($proposal_header, $proposal_rows);
  return $return_html;
}

function lab_migration_solution_proposal_form($form, $form_state)
{
  global $user;

  $proposal_id = (int)arg(2);

  //$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  $query = db_select('lab_migration_proposal');
           $query->fields('lab_migration_proposal');
           $query->condition('id', $proposal_id);
           $proposal_q = $query->execute();
  $proposal_data = $proposal_q->fetchObject();
  if (!$proposal_data)
  {
    drupal_set_message("Invalid proposal.", 'error');
    drupal_goto('');
  }
    //var_dump($proposal_data->name); die;
  $form['name'] = array(
    '#type' => 'item',
    '#markup' => l($proposal_data->name_title . ' ' . $proposal_data->name, 'user/' . $proposal_data->uid),
    '#title' => t('Proposer Name'),
  );
  $form['lab_title'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->lab_title,
    '#title' => t('Title of the Lab'),
  );

  $experiment_html = '';
  //$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d", $proposal_id);
  $query = db_select('lab_migration_experiment');
  $query->fields('lab_migration_experiment');
  $query->condition('proposal_id', $proposal_id);
  $experiment_q = $query->execute();
  while ($experiment_data = $experiment_q->fetchObject()) {
 		$experiment_html .= $experiment_data->title . "<br/>";
	}
  $form['experiment'] = array(
    '#type' => 'item',
    '#markup' => $experiment_html,
    '#title' => t('Experiment List'),
  );

  $form['solution_provider_name_title'] = array(
    '#type' => 'select',
    '#title' => t('Title'),
    '#options' => array('Mr' => 'Mr', 'Ms' => 'Ms', 'Mrs' => 'Mrs', 'Dr' => 'Dr', 'Prof' => 'Prof'),
    '#required' => TRUE,
  );
  $form['solution_provider_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name of the Solution Provider'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
  );
  $form['solution_provider_email_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#size' => 30,
    '#value' => $user->mail,
    '#disabled' => TRUE,
  );
  $form['solution_provider_contact_ph'] = array(
    '#type' => 'textfield',
    '#title' => t('Contact No.'),
    '#size' => 30,
    '#maxlength' => 15,
    '#required' => TRUE,
  );
  $form['solution_provider_department'] = array(
    '#type' => 'select',
    '#title' => t('Department/Branch'),
    '#options' => array('' => 'Please select...',
                        'Computer Engineering' => 'Computer Engineering',
                        'Electrical Engineering' => 'Electrical Engineering',
                        'Electronics Engineering' => 'Electronics Engineering',
                        'Chemical Engineering' => 'Chemical Engineering',
                        'Instrumentation Engineering' => 'Instrumentation Engineering',
                        'Mechanical Engineering' => 'Mechanical Engineering',
                        'Civil Engineering' => 'Civil Engineering',
                        'Physics' => 'Physics',
                        'Mathematics' => 'Mathematics',
                        'Others' => 'Others'),
    '#required' => TRUE,
  );
  $form['solution_provider_university'] = array(
    '#type' => 'textfield',
    '#title' => t('University/Institute'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
  );

$form['samplefile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sample Source File'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
	
  );
  $form['samplefile']['samplefile1'] = array(
      '#type' => 'file',
      '#title' => t('Upload sample source file'),
      '#size' => 48,  
      '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
      t('<span style="color:red;">Allowed file extensions : ') . variable_get('textbook_companion_source_extensions', '').'</span>',
     
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Apply for Solution')
  );
  return $form;
}

function lab_migration_solution_proposal_form_validate($form, &$form_state)
{
	global $user;

	//$solution_provider_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE solution_provider_uid = ".$user->uid." AND approval_status IN (0, 1) AND solution_status IN (0, 1, 2)");
  $query = db_select('lab_migration_proposal');
            $query->fields('lab_migration_proposal');
            $query->condition('solution_provider_uid', $user->uid);
            $query->condition('approval_status', array(0, 1), 'IN');
            $query->condition('solution_status', array(0, 1, 2), 'IN');
            $solution_provider_q = $query->execute();
	if ($solution_provider_q->fetchObject()) {
    form_set_error('', t("You have already applied for a solution. Please complete that before applying for another solution."));
    drupal_goto('lab_migration/open_proposal');
	}

if ( ! ($_FILES['files']['name']['samplefile1'])){
      form_set_error('samplefile1', t('Please upload sample code main or source file.'));
}

if (isset($_FILES['files']))
  {
    /* check if atleast one source or result file is uploaded */  
    if ( ! ($_FILES['files']['name']['samplefile1']))
      form_set_error('samplefile1', t('Please upload sample code main or source file.'));

    /* check for valid filename extensions */  
    foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
    {
      if ($file_name)
      {
        /* checking file type */
        if (strstr($file_form_name, 'sample'))
          $file_type = 'S';
        else
          $file_type = 'U';

        $allowed_extensions_str = '';
        switch ($file_type)
        {
        case 'S':
          $allowed_extensions_str = variable_get('lab_migration_source_extensions', '');
          break;
	
        }
        $allowed_extensions = explode(',' , $allowed_extensions_str);
        $temp_extension = end(explode('.', strtolower($_FILES['files']['name'][$file_form_name])));
        if (!in_array($temp_extension, $allowed_extensions))
          form_set_error($file_form_name, t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
        if ($_FILES['files']['size'][$file_form_name] <= 0)
          form_set_error($file_form_name, t('File size cannot be zero.'));

        /* check if valid file name */
        if (!lab_migration_check_valid_filename($_FILES['files']['name'][$file_form_name]))
         form_set_error($file_form_name, t('Invalid file name specified. Only alphabets and numbers are allowed as a valid filename.'));
      }
    }
  }

}

function lab_migration_solution_proposal_form_submit($form, &$form_state)
{
  global $user;
	$root_path =lab_migration_samplecode_path();
  $proposal_id = (int)arg(2);

  //$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  $query = db_select('lab_migration_proposal');
            $query->fields('lab_migration_proposal');
            $query->condition('id', $proposal_id);
            $proposal_q = $query->execute();
  $proposal_data = $proposal_q->fetchObject();
  if (!$proposal_data)
  {
    drupal_set_message("Invalid proposal.", 'error');
    drupal_goto('lab_migration/open_proposal');
  }
  if ($proposal_data->solution_provider_uid != 0) {
    drupal_set_message("Someone has already applied for solving this Lab.", 'error');
    drupal_goto('lab_migration/open_proposal');
  }
	$actual_path="";
  $dest_path = $proposal_id . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);


      /* uploading files */
  foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
  {
    if ($file_name)
    {
       /* checking file type */
      $file_type = 'S';

      if (file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
      {
        drupal_set_message(t("Error uploading file. File !filename already exists.", array('!filename' => $_FILES['files']['name'][$file_form_name])), 'error');
	drupal_goto('lab_migration/open_proposal');
        return;
      }

      /* uploading file */
      if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
      {
        $actual_path=$dest_path . $_FILES['files']['name'][$file_form_name];
    
        drupal_set_message($file_name . ' uploaded successfully.', 'status');
      } else {
        drupal_set_message('Error uploading file : ' . $dest_path . '/' . $file_name, 'error');
	drupal_goto('lab_migration/open_proposal');
      }
    }
  }


  $query = "UPDATE {lab_migration_proposal} set solution_provider_uid = :uid, solution_status = 1, solution_provider_name_title = :solution_provider_name_title, solution_provider_name = :solution_provider_contact_name, solution_provider_contact_ph = :solution_provider_contact_ph, solution_provider_department = :solution_provider_department, solution_provider_university = :solution_provider_university,samplefilepath=:samplefilepath WHERE id = :proposal_id";
    $args = array(
              	":uid" => $user->uid,
              	":solution_provider_name_title" => $form_state['values']['solution_provider_name_title'],
              	":solution_provider_contact_name" => $form_state['values']['solution_provider_name'],
              	":solution_provider_contact_ph" => $form_state['values']['solution_provider_contact_ph'],
              	":solution_provider_department" => $form_state['values']['solution_provider_department'],
              	":solution_provider_university" => $form_state['values']['solution_provider_university'],
		":samplefilepath" => $actual_path,
              	":proposal_id" => $proposal_id
             );
    $result = db_query($query, $args);
	drupal_set_message("We have received your application. We will get back to you soon.", 'status');

  /* sending email */
  $email_to = $user->mail;

  $from = variable_get('lab_migration_from_email', '');
  $bcc= variable_get('lab_migration_emails', '');
  $cc=variable_get('lab_migration_cc_emails', '');  

  $param['solution_proposal_received']['proposal_id'] = $proposal_id;
  $param['solution_proposal_received']['user_id'] = $user->uid;
  $param['solution_proposal_received']['headers']=array('From'=>$from,'MIME-Version'=> '1.0',
    			'Content-Type'=> 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
    			'Content-Transfer-Encoding' => '8Bit',
    			'X-Mailer'=> 'Drupal','Cc' => $cc, 'Bcc' => $bcc);

  if (!drupal_mail('lab_migration', 'solution_proposal_received', $email_to , language_default(), $param, $from, TRUE))
    drupal_set_message('Error sending email message.', 'error');

  /* sending email */
 /* $email_to = variable_get('lab_migration_emails', '');
  if (!drupal_mail('lab_migration', 'solution_proposal_received', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
    drupal_set_message('Error sending email message.', 'error');*/

	drupal_goto('lab_migration/open_proposal');
}

