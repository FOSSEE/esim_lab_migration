<?php
// $Id$

function lab_migration_solution_proposal_pending()
{
  /* get list of solution proposal where the solution_provider_uid is set to some userid except 0 and solution_status is also 1 */
  $pending_rows = array();
  //$pending_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE solution_provider_uid != 0 AND solution_status = 1 ORDER BY id DESC");
  $query = db_select('lab_migration_proposal');
            $query->fields('lab_migration_proposal');
            $query->condition('solution_provider_uid', 0, '!=');
            $query->condition('solution_status', 1);
            $query->orderBy('id', 'DESC');
            $pending_q = $query->execute();
  while ($pending_data = $pending_q->fetchObject())
  {
    $pending_rows[$pending_data->id] = array(l($pending_data->name, 'user/' . $pending_data->uid), $pending_data->lab_title, l('Approve', 'lab_migration/manage_proposal/solution_proposal_approve/' . $pending_data->id));
  }

  /* check if there are any pending proposals */
  if (!$pending_rows)
  {
    drupal_set_message(t('There are no pending solution proposals.'), 'status');
    return '';
  }

  $pending_header = array('Proposer Name', 'Title of the Lab', 'Action');  
  $output = theme('table', array('header' => $pending_header, 'rows' => $pending_rows ));
  return $output; 
}

/******************************************************************************/
/***************** SOLUTION PROPOSAL APPROVAL FORM ****************************/
/******************************************************************************/

function lab_migration_solution_proposal_approval_form($form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
 // $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  $query = db_select('lab_migration_proposal');
            $query->fields('lab_migration_proposal');
            $query->condition('id', $proposal_id);
            $proposal_q = $query->execute();
  if ($proposal_q)
  {
    if ($proposal_data = $proposal_q->fetchObject())
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('lab_migration/manage_proposal/pending_solution_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal/pending_solution_proposal');
    return;
  }

  $form['name'] = array(
    '#type' => 'item',
    '#markup' => l($proposal_data->name_title . ' ' . $proposal_data->name, 'user/' . $proposal_data->uid),
    '#title' => t('Proposer Name'),
  );
  $form['email_id'] = array(
    '#type' => 'item',
    '#markup' => user_load($proposal_data->uid)->mail,
    '#title' => t('Email'),
  );
  $form['contact_ph'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->contact_ph,
    '#title' => t('Contact No.'),
  );
  $form['department'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->department,
    '#title' => t('Department/Branch'),
  );
  $form['university'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->university,
    '#title' => t('University/Institute'),
  );

$form['country'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->country,
        '#title' => t('Country')
    );
    $form['all_state'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->state,
        '#title' => t('State')
    );
    $form['city'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->city,
        '#title' => t('City')
    );
    $form['pincode'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->pincode,
        '#title' => t('Pincode/Postal code')
    );


 $form['esim_version'] = array(
    '#type' => 'item',
    '#title' => t('eSim version used'),
    '#markup'=>$proposal_data->esim_version,
  );


  $form['lab_title'] = array(
    '#type' => 'item',
    '#markup' => $proposal_data->lab_title,
    '#title' => t('Title of the Lab'),
  );

  /* get experiment details */
  $experiment_list = '<ul>';
  //$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY id ASC", $proposal_id);
  $query = db_select('lab_migration_experiment');
            $query->fields('lab_migration_experiment');
            $query->condition('proposal_id', $proposal_id);
            $query->orderBy('id', 'ASC');
            $experiment_q = $query->execute();
  while ($experiment_data = $experiment_q->fetchObject())
  {
    $experiment_list .= '<li>' . $experiment_data->title . '</li>Description of Experiment : '.$experiment_data->description.'<br>';;
  }
  $experiment_list .= '</ul>';

  $form['experiment'] = array(
      '#type' => 'item',
      '#markup' => $experiment_list,
      '#title' => t('Experiments'),
  );

  $form['solution_display'] = array(
	  '#type' => 'hidden',
		'#title' => t('Display the solution on the www.esim.fossee.in website'),
		'#markup' => ($proposal_data->solution_display == 1) ? "Yes" : "No",
  );

  if ($proposal_data->solution_provider_uid == 0) {
  	$solution_provider = "Open";
  } else if ($proposal_data->solution_provider_uid == $proposal_data->uid) {
  	$solution_provider = "Proposer will provide the solution of the lab";
  } else {
  	$solution_provider_user_data = user_load($proposal_data->solution_provider_uid);
  	if ($solution_provider_user_data) {
  		$solution_provider .= '<ul>' .
        '<li><strong>Solution Provider:</strong> ' . l($solution_provider_user_data->name, 'user/' . $proposal_data->solution_provider_uid) . '</li>' .
        '<li><strong>Solution Provider Name:</strong> ' . $proposal_data->solution_provider_name_title . ' ' . $proposal_data->solution_provider_name . '</li>' .
        '<li><strong>Department:</strong> ' . $proposal_data->solution_provider_department . '</li>' .
        '<li><strong>University:</strong> ' . $proposal_data->solution_provider_university . '</li>' .
        '</ul>';
  	} else {
  		$solution_provider = "User does not exists";
  	}
  }
  $form['solution_provider_uid'] = array(
	  '#type' => 'item',
		'#title' => t('Solution Provider'),
		'#markup' => $solution_provider,
  );

if($proposal_data->samplefilepath !="None"){
$form['samplecode'] = array(
      '#type' => 'markup',
      '#markup' => l('Download Sample Code', 'lab_migration/download/samplecode/' . $proposal_id)."<br><br>" ,
    );
}
  $form['approval'] = array(
	  '#type' => 'radios',
		'#title' => t('Solution Provider'),
		'#options' => array('1' => 'Approve', '2' => 'Disapprove'),
		'#required' => TRUE,
	);

  $form['message'] = array(	
    '#type' => 'textarea',
    '#title' => t('Reason for disapproval'),
	'#states' => array(
            'visible' => array(
            ':input[name="approval"]' => array('value' => '2'),
            ),
	    'required' => array(
   	    ':input[name="approval"]' =>  array('value' => '2')
	    ),
       	),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'lab_migration/manage_proposal/pending_solution_proposal'),
  );

  return $form;
}

function lab_migration_solution_proposal_approval_form_validate($form, &$form_state)
{
  $proposal_id = (int)arg(3);

 // $solution_provider_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  $query = db_select('lab_migration_proposal');
          $query->fields('lab_migration_proposal');
          $query->condition('id', $proposal_id);
          $solution_provider_q = $query->execute();
  $solution_provider_data = $solution_provider_q->fetchObject();

 //	$solution_provider_present_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE solution_provider_uid = %d AND approval_status IN (0, 1) AND id != %d", $solution_provider_data->uid, $proposal_id);

  $query = db_select('lab_migration_proposal');
            $query->fields('lab_migration_proposal');
            $query->condition('solution_provider_uid', $solution_provider_data->uid);
            $query->condition('approval_status', array(0, 1), 'IN');
            $query->condition('id', $proposal_id, '<>');
            $solution_provider_present_q = $query->execute();
	if ($x = $solution_provider_present_q->fetchObject()) {
		drupal_set_message($proposal_id);
		form_set_error('', t('Solution provider has already one proposal active'));
	}

	if( $form_state['values']['approval']==2){
		if(strlen(trim($form_state['values']['message'])) <=30){
		form_set_error('message', t('Please mention the reason for disapproval.'));
		}
	}
}

function lab_migration_solution_proposal_approval_form_submit($form, &$form_state)
{
	global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  //$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  $query = db_select('lab_migration_proposal');
           $query->fields('lab_migration_proposal');
           $query->condition('id', $proposal_id);
           $proposal_q = $query->execute();
  if ($proposal_q)
  {
    if ($proposal_data = $proposal_q->fetchObject())
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('lab_migration/manage_proposal/pending_solution_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal/pending_solution_proposal');
    return;
  }

  $user_data = user_load($proposal_data->solution_provider_uid);

  if ($form_state['values']['approval'] == 1) {
  	$query = "UPDATE {lab_migration_proposal} SET solution_status = 2 WHERE id =:proposal_id";
    $args = array(
                   ":proposal_id" => $proposal_id
            );
    db_query($query, $args);

    /* sending email */
    $email_to = $user_data->mail;

    $from = variable_get('lab_migration_from_email', '');
    $bcc= $user->mail . ', ' .variable_get('lab_migration_emails', '');
    $cc=variable_get('lab_migration_cc_emails', '');

    $param['solution_proposal_approved']['proposal_id'] = $proposal_id;
    $param['solution_proposal_approved']['user_id'] = $proposal_data->solution_provider_uid;
    $param['solution_proposal_approved']['headers']=array('From'=>$from,'MIME-Version'=> '1.0',
    			'Content-Type'=> 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
    			'Content-Transfer-Encoding' => '8Bit',
    			'X-Mailer'=> 'Drupal','Cc' => $cc, 'Bcc' => $bcc);


    if (!drupal_mail('lab_migration', 'solution_proposal_approved', $email_to , language_default(), $param, $from, TRUE))
      drupal_set_message('Error sending email message.', 'error');

    /*$email_to = $user->mail . ', ' . variable_get('lab_migration_emails', '');
    if (!drupal_mail('lab_migration', 'solution_proposal_approved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');*/

    drupal_set_message('Lab migration solution proposal approved. User has been notified of the approval.', 'status');
    drupal_goto('lab_migration/manage_proposal/pending_solution_proposal');
    return;
  } else if ($form_state['values']['approval'] == 2) {
    $query = "UPDATE {lab_migration_proposal} SET solution_provider_uid = :solution_provider_uid, solution_status = :solution_status, solution_provider_name_title = '', solution_provider_name = '', solution_provider_contact_ph = '', solution_provider_department = '', solution_provider_university = '' WHERE id = :proposal_id";

        $args = array( 
                   ":solution_provider_uid" => 0,
                   ":solution_status" => 0,                  
                   ":proposal_id" => $proposal_id
                );
    db_query($query, $args);

    /* sending email */
    $email_to = $user_data->mail;

    $from = variable_get('lab_migration_from_email', '');
    $bcc= $user->mail . ', ' .variable_get('lab_migration_emails', '');
    $cc=variable_get('lab_migration_cc_emails', '');

    $param['solution_proposal_disapproved']['proposal_id'] = $proposal_id;
    $param['solution_proposal_disapproved']['user_id'] = $proposal_data->solution_provider_uid;
    $param['solution_proposal_disapproved']['message'] = $form_state['values']['message'];
    $param['solution_proposal_disapproved']['headers']=array('From'=>$from,'MIME-Version'=> '1.0',
    			'Content-Type'=> 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
    			'Content-Transfer-Encoding' => '8Bit',
    			'X-Mailer'=> 'Drupal','Cc' => $cc, 'Bcc' => $bcc);

    if (!drupal_mail('lab_migration', 'solution_proposal_disapproved', $email_to , language_default(), $param, $from, TRUE))
      drupal_set_message('Error sending email message.', 'error');

    /*$email_to = $user->mail . ', ' . variable_get('lab_migration_emails', '');;
    if (!drupal_mail('lab_migration', 'solution_proposal_disapproved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');*/

    drupal_set_message('Lab migration solution proposal dis-approved. User has been notified of the dis-approval.', 'status');
    drupal_goto('lab_migration/manage_proposal/pending_solution_proposal');
    return;
  }
}

