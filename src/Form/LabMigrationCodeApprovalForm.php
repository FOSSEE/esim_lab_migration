<?php

/**
 * @file
 * Contains \Drupal\lab_migration\Form\LabMigrationCodeApprovalForm.
 */

namespace Drupal\lab_migration\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;
use Drupal\Core\Url;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Symfony\Component\HttpFoundation\Response;
use Drupal\user\Entity\User;
use Drupal\Core\Link;
use Drupal\Core\Config\ConfigFactoryInterface;

class LabMigrationCodeApprovalForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'lab_migration_code_approval_form';
  }

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    // $solution_id = (int) arg(3);
    $route_match = \Drupal::routeMatch();

    $solution_id = (int) $route_match->getParameter('solution_id');
   
    /* get solution details */
    //$solution_q = \Drupal::database()->query("SELECT * FROM {lab_migration_solution} WHERE id = %d", $solution_id);
    $query = \Drupal::database()->select('lab_migration_solution');
    $query->fields('lab_migration_solution');
    $query->condition('id', $solution_id);
    $solution_q = $query->execute();
    $solution_data = $solution_q->fetchObject();
    if (!$solution_data) {
      \Drupal::messenger()->addMessage(t('Invalid solution selected.'), 'status');
      // drupal_goto('lab_migration/code_approval');
       // RedirectResponse('lab-migration/code-approval');
       $url = Url::fromRoute('lab_migration.code_approval')->toString(); // Replace with the actual route name
       $response = new RedirectResponse($url);
       
       // Return the redirect response
       return $response;
    }
    if ($solution_data->approval_status == 1) {
      \Drupal::messenger()->addMessage(t('This solution has already been approved. Are you sure you want to change the approval status?'), 'error');
    }
    if ($solution_data->approval_status == 2) {
      \Drupal::messenger()->addMessage(t('This solution has already been dis-approved. Are you sure you want to change the approval status?'), 'error');
    }

    /* get experiment data */
    //xperiment_q = \Drupal::database()->query("SELECT * FROM {lab_migration_experiment} WHERE id = %d", $solution_data->experiment_id);
    $query = \Drupal::database()->select('lab_migration_experiment');
    $query->fields('lab_migration_experiment');
    $query->condition('id', $solution_data->experiment_id);
    $experiment_q = $query->execute();
    $experiment_data = $experiment_q->fetchObject();

    /* get proposal data */
    //$proposal_q = \Drupal::database()->query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $experiment_data->proposal_id);
    $query = \Drupal::database()->select('lab_migration_proposal');
    $query->fields('lab_migration_proposal');
    $query->condition('id', $experiment_data->proposal_id);
    $proposal_q = $query->execute();
    $proposal_data = $proposal_q->fetchObject();

    /* get solution provider details */
    $solution_provider_user_name = '';
    $user_data = User::load($proposal_data->solution_provider_uid);
    if ($user_data) {
      $solution_provider_user_name = $user_data->name;
    }
    else {
      $solution_provider_user_name = '';
    }

    $form['#tree'] = TRUE;

    $form['lab_title'] = [
      '#type' => 'item',
      '#markup' => $proposal_data->lab_title,
      '#title' => t('Title of the Lab'),
    ];

    $form['name'] = [
      '#type' => 'item',
      '#markup' => $proposal_data->name,
      '#title' => t('Contributor Name'),
    ];

    $form['experiment']['number'] = [
      '#type' => 'item',
      '#markup' => $experiment_data->number,
      '#title' => t('Experiment Number'),
    ];

    $form['experiment']['title'] = [
      '#type' => 'item',
      '#markup' => $experiment_data->title,
      '#title' => t('Title of the Experiment'),
    ];

    $form['back_to_list'] = [
      '#type' => 'item',
      '#markup' => Link::fromTextAndUrl('Back to Code Approval List', 
      Url::fromRoute('lab_migration.code_approval'))->toString(),
      // '#markup' => l('Back to Code Approval List', 'lab_migration/code_approval'),
    ];

    $form['code_number'] = [
      '#type' => 'item',
      '#markup' => $solution_data->code_number,
      '#title' => t('Code No'),
    ];

    $form['code_caption'] = [
      '#type' => 'item',
      '#markup' => $solution_data->caption,
      '#title' => t('Caption'),
    ];


   
    // ===
    $solution_files_html = '';

    $query = \Drupal::database()->select('lab_migration_solution_files', 's')
      ->fields('s')
      ->condition('solution_id', $solution_id)
      ->orderBy('id', 'ASC');
    
    $solution_files_q = $query->execute();
    
    foreach ($solution_files_q as $solution_files_data) {
      $code_file_type = match ($solution_files_data->filetype) {
        'S' => 'Source',
        'R' => 'Result',
        'X' => 'Xcox',
        'U' => 'Unknown',
        default => 'Unknown',
      };
    
      // 1️⃣ Solution file link
      $file_url = Url::fromUri('internal:/lab-migration/download/file/' . $solution_files_data->id);
      $file_link = Link::fromTextAndUrl($solution_files_data->filename, $file_url)->toString();
    
      $solution_files_html .= $file_link . ' (' . $code_file_type . ')<br/>';
    
      // 2️⃣ If PDF exists, add PDF link
      if (strlen($solution_files_data->pdfpath) >= 5) {
        $pdfname = substr($solution_files_data->pdfpath, strrpos($solution_files_data->pdfpath, '/') + 1);
        $pdf_url = Url::fromUri('internal:/lab-migration/download/pdf/' . $solution_files_data->id);
        $pdf_link = Link::fromTextAndUrl($pdfname, $pdf_url)->toString();
        $solution_files_html .= $pdf_link . ' (PDF File)<br/>';
      }
    }
    
    

    $form['solution_files'] = [
      '#type' => 'item',
      '#markup' => $solution_files_html,
      '#title' => t('Solution'),
    ];
        $form['approved'] = [
      '#type' => 'radios',
      '#options' => [
        '0' => 'Pending',
        '1' => 'Approved',
        '2' => 'Dis-approved (Solution will be deleted)',
      ],
      '#title' => t('Approval'),
      '#default_value' => $solution_data->approval_status,
    ];

    $form['message'] = [
      '#type' => 'textarea',
      '#title' => t('Reason for dis-approval'),
      '#states' => [
        'visible' => [
          ':input[name="approved"]' => [
            'value' => '2'
            ]
          ],
        'required' => [':input[name="approved"]' => ['value' => '2']],
      ],
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => t('Submit'),
    ];

    $form['cancel'] = [
      '#type' => 'markup',
      
      // '#markup' => l(t('Cancel'), 'lab_migration/code_approval'),
    '#markup' => Link::fromTextAndUrl( t('Cancel'), 
         Url::fromRoute('lab_migration.code_approval'))->toString(),

    ];

    return $form;
  }

  public function validateForm(array &$form, \Drupal\Core\Form\FormStateInterface $form_state) {
    if ($form_state->getValue(['approved']) == 2) {
      if (strlen(trim($form_state->getValue(['message']))) <= 30) {
        $form_state->setErrorByName('message', t('Please mention the reason for disapproval.'));
      }
    }
    return;

  }

  public function submitForm(array &$form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $user = \Drupal::currentUser();
    $approver_uid = $user->id();
  
    if (empty($approver_uid)) {
      \Drupal::messenger()->addMessage('Approver user ID is missing.', 'error');
      return;
    }
  
    $route_match = \Drupal::routeMatch();
    $solution_id = (int) $route_match->getParameter('solution_id');
  
    // Get solution details.
    $query = \Drupal::database()->select('lab_migration_solution');
    $query->fields('lab_migration_solution');
    $query->condition('id', $solution_id);
    $solution_data = $query->execute()->fetchObject();
    if (!$solution_data) {
      \Drupal::messenger()->addMessage(t('Invalid solution selected.'), 'error');
      return;
    }
  
    // Get proposal for emails.
    $query = \Drupal::database()->select('lab_migration_experiment');
    $query->fields('lab_migration_experiment');
    $query->condition('id', $solution_data->experiment_id);
    $experiment_data = $query->execute()->fetchObject();
  
    $query = \Drupal::database()->select('lab_migration_proposal');
    $query->fields('lab_migration_proposal');
    $query->condition('id', $experiment_data->proposal_id);
    $proposal_data = $query->execute()->fetchObject();
  
    $user_data = User::load($proposal_data->uid);
    $email_to = $user_data ? $user_data->getEmail() : '';
  
    // Example: get `from` email from config.
    $from = \Drupal::config('system.site')->get('mail');
    $param = [];
  
    // === Handle approvals ===
    $approval_value = $form_state->getValue(['approved']);
    if ($approval_value == "0") {
      \Drupal::database()->update('lab_migration_solution')
        ->fields([
          'approval_status' => 0,
          'approver_uid' => $approver_uid,
          'approval_date' => time(),
        ])
        ->condition('id', $solution_id)
        ->execute();
  
      // TODO: Send pending email if needed
    }
    elseif ($approval_value == "1") {
      \Drupal::database()->update('lab_migration_solution')
        ->fields([
          'approval_status' => 1,
          'approver_uid' => $approver_uid,
          'approval_date' => time(),
        ])
        ->condition('id', $solution_id)
        ->execute();
  
      // TODO: Send approved email if needed
    }
    elseif ($approval_value == "2") {
      if (lab_migration_delete_solution($solution_id)) {
        // TODO: Send disapproval email if needed
      }
      else {
        \Drupal::messenger()->addMessage('Error disapproving and deleting solution.', 'error');
      }
    }
  
    \Drupal::messenger()->addMessage('Updated successfully.', 'status');
    $response = new RedirectResponse(Url::fromRoute('lab_migration.code_approval')->toString());
    $response->send();
  }
  

}
?>
